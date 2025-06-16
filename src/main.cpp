#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <RTClib.h>
#include "DHT.h"
#include <iostream>
#include <algorithm>
using namespace std;

BH1750 lightMeter;
RTC_DS3231 rtc;

#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int sensor_pin = 34;
const int relayPin = 26;

void setup() {
    Serial.begin(9600);

    Wire.begin(21, 22);

    dht.begin();

    // Init sensors
    if (!lightMeter.begin()) {
    Serial.println("BH1750 not found");
    }

    if (!rtc.begin()) {
    Serial.println("DS3231 not found");
    }

    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, HIGH);
  // Optionally set time on RTC
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

// === Fuzzy Mamdani ===
// ===== Fuzifikasi ====
// ======== SUHU ========
float suhu_dingin(float x) {
    if (x <= 15) return 1;
    else if (x > 15 && x < 20) return (20 - x) / 5.0;
    else return 0;
}

float suhu_normal(float x) {
    if (x <= 15 || x >= 30) return 0;
    else if (x > 15 && x < 20) return (x - 15) / 5.0;
    else if (x > 25 && x < 30) return (30 - x) / 5.0;
    else return 1;
}

float suhu_panas(float x) {
    if (x <= 25) return 0;
    else if (x > 25 && x < 30) return (x - 25) / 5.0;
    else return 1;
}

// ======== KELEMBAPAN TANAH ========
float lembap_kering(float x) {
    if (x <= 40) return 1;
    else if (x > 40 && x < 50) return (50 - x) / 10.0;
    else return 0;
}

float lembap_sedang(float x) {
    if (x <= 40 || x >= 70) return 0;
    else if (x > 40 && x < 50) return (x - 40) / 10.0;
    else if (x > 60 && x < 70) return (70 - x) / 10.0;
    else return 1;
}

float lembap_basah(float x) {
    if (x <= 60) return 0;
    else if (x > 60 && x < 70) return (x - 60) / 10.0;
    else return 1;
}

// ======== INTENSITAS CAHAYA ========
float cahaya_redup(float x) {
    if (x <= 300) return 1;
    else if (x < 300 ) return (300 - x) / 200.0;
    else return 0;
}

float cahaya_sedang(float x) {
    if (x <= 150 || x >= 1200) return 0;
    else if (x > 150 && x < 500) return (x - 150) / 350.0;
    else if (x > 700  && x < 1200) return (1200 - x) / 500.0;
    else return 1;
}

float cahaya_terang(float x) {
    if (x <= 800) return 0;
    else if (x < 1500) return (x - 800) / 700.0;
    else return 1;
}

//INFERENSI
// === Fungsi MIN dan MAX ===
float fuzzy_and(float a, float b) {
    return min(a, b);
}

float fuzzy_and3(float a, float b, float c) {
    return min({a, b, c});
}

float fuzzy_or(float a, float b) {
    return max(a, b);
}

float fuzzy_or4(float a, float b, float c, float d) {
    return max({a, b, c, d});
}

void loop() {
    float cahaya = lightMeter.readLightLevel();
    DateTime now = rtc.now();
    float h = dht.readHumidity();
    float suhu = dht.readTemperature();
    int sensor_analog = analogRead(sensor_pin);
    int kelembapan = 100 - ((sensor_analog * 100) / 4095);

    int jam = now.hour();
    int menit = now.minute();
    int detik = now.second();

    if ((jam == 15 || jam == 16) && menit == 55 && detik == 0) {

        // === Fuzzifikasi ===
        float mu_suhu_dingin = suhu_dingin(suhu);
        float mu_suhu_normal = suhu_normal(suhu);
        float mu_suhu_panas = suhu_panas(suhu);

        float mu_lembap_kering = lembap_kering(kelembapan);
        float mu_lembap_sedang = lembap_sedang(kelembapan);
        float mu_lembap_basah  = lembap_basah(kelembapan);

        float mu_cahaya_redup  = cahaya_redup(cahaya);
        float mu_cahaya_sedang = cahaya_sedang(cahaya);
        float mu_cahaya_terang = cahaya_terang(cahaya);

        float r[27];
        int i = 0;

        // Suhu Dingin
        r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_kering, mu_cahaya_redup);  // 1
        r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_kering, mu_cahaya_sedang); // 2
        r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_kering, mu_cahaya_terang);  // 3
        r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_sedang, mu_cahaya_redup);  // 4
        r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_sedang, mu_cahaya_sedang); // 5
        r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_sedang, mu_cahaya_terang);  // 6
        r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_basah, mu_cahaya_redup);   // 7
        r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_basah, mu_cahaya_sedang);  // 8
        r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_basah, mu_cahaya_terang);   // 9

        // Suhu Normal
        r[i++] = fuzzy_and3(mu_suhu_normal, mu_lembap_kering, mu_cahaya_redup);   // 10
        r[i++] = fuzzy_and3(mu_suhu_normal, mu_lembap_kering, mu_cahaya_sedang);  // 11
        r[i++] = fuzzy_and3(mu_suhu_normal, mu_lembap_kering, mu_cahaya_terang);   // 12
        r[i++] = fuzzy_and3(mu_suhu_normal, mu_lembap_sedang, mu_cahaya_redup);   // 13
        r[i++] = fuzzy_and3(mu_suhu_normal, mu_lembap_sedang, mu_cahaya_sedang);  // 14
        r[i++] = fuzzy_and3(mu_suhu_normal, mu_lembap_sedang, mu_cahaya_terang);   // 15
        r[i++] = fuzzy_and3(mu_suhu_normal, mu_lembap_basah, mu_cahaya_redup);    // 16
        r[i++] = fuzzy_and3(mu_suhu_normal, mu_lembap_basah, mu_cahaya_sedang);   // 17
        r[i++] = fuzzy_and3(mu_suhu_normal, mu_lembap_basah, mu_cahaya_terang);    // 18

        // Suhu Panas
        r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_kering, mu_cahaya_redup);   // 19
        r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_kering, mu_cahaya_sedang);  // 20
        r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_kering, mu_cahaya_terang);   // 21
        r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_sedang, mu_cahaya_redup);   // 22 
        r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_sedang, mu_cahaya_sedang);  // 23
        r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_sedang, mu_cahaya_terang);   // 24
        r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_basah, mu_cahaya_redup);    // 25
        r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_basah, mu_cahaya_sedang);   // 26
        r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_basah, mu_cahaya_terang);    // 27

        // Output fuzzy untuk masing-masing aturan
        // Gabungkan berdasarkan output
        float mu_tidak = max({r[3], r[4], r[5], r[6], r[7], r[8], r[15], r[16], r[17], r[24], r[25], r[26]});
        float mu_cepat = max({r[2], r[11], r[14], r[20], r[23]}); // disiram cepat (2, 11, 14, 20, 23)
        float mu_sedang = max({r[1], r[12], r[13], r[21], r[22]}); // disiram sedang (1,12,13,21,22)
        float mu_lama = max({r[0], r[9], r[10], r[18], r[19]}); // disiram lama (0,9,10,18,19)

        //tumbuhan kecil
        float z_tidak = 0;
        float z_cepat = 1;
        float z_sedang = 2;
        float z_lama = 3;

        //tumbuhan kecil
        // float z_tidak = 0;
        // float z_cepat = 5;
        // float z_sedang = 10;
        // float z_lama = 20;

        float numerator = (mu_tidak * z_tidak) +
                            (mu_sedang * z_sedang) +
                            (mu_cepat * z_cepat)+
                            (mu_lama * z_lama);

        float denominator = mu_tidak + mu_sedang + mu_cepat + mu_lama;

        float z_output = (denominator == 0) ? 0 : numerator / denominator;

        // cout << "\n>>> Nilai akhir penyiraman (detik): " << z_output << " detik" << endl;

        Serial.println("====== DATA SENSOR ======");
        Serial.print("Suhu: "); Serial.print(suhu); Serial.println(" °C");
        Serial.print("Kelembapan Tanah: "); Serial.print(kelembapan); Serial.println(" %");
        Serial.print("Cahaya: "); Serial.print(cahaya); Serial.println(" lux");
        Serial.print(z_output); Serial.println(" detik");
        if (z_output > 0) {
            digitalWrite(relayPin, LOW);      // Nyalakan pompa
            delay(z_output * 1000);            // Tunggu sesuai durasi (dalam ms)
            digitalWrite(relayPin, HIGH);       // Matikan pompa
        }
        Serial.println("=========================\n");
        delay(1000);
    } else {
        Serial.print(jam); Serial.print(":"); Serial.print(menit); Serial.print(":"); Serial.println(detik);
        delay(500);
    }
}