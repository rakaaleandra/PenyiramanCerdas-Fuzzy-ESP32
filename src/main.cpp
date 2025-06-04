#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <RTClib.h>
#include "DHT.h"

BH1750 lightMeter;
RTC_DS3231 rtc;

#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int sensor_pin = 34;

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

  // Optionally set time on RTC
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

// === Fungsi Fuzzy Kategori ===
String suhuKategori(float suhu) {
  if (suhu < 25) return "Dingin";
  else if (suhu < 32) return "Sejuk";
  else return "Panas";
}

String kelembapanTanahKategori(float persenTanah) {
  if (persenTanah < 30) return "Kering";
  else if (persenTanah < 70) return "Lembab";
  else return "Basah";
}

String cahayaKategori(uint16_t lux) {
  if (lux < 1000) return "Redup";
  else if (lux < 10000) return "Sedang";
  else return "Terik";
}

// === Fungsi Fuzzy Decision ===
String fuzzyKeputusan(String suhu, String tanah, String cahaya) {
  if (suhu == "Dingin" && tanah == "Kering" && cahaya == "Redup") return "Lama";
  if (suhu == "Dingin" && tanah == "Kering" && cahaya == "Sedang") return "Sedang";
  if (suhu == "Dingin" && tanah == "Kering" && cahaya == "Terik") return "Cepat";

  if (suhu == "Dingin" && tanah == "Lembab") return "Tidak Disiram";
  if (suhu == "Dingin" && tanah == "Basah") return "Tidak Disiram";

  if (suhu == "Sejuk" && tanah == "Kering" && cahaya == "Redup") return "Lama";
  if (suhu == "Sejuk" && tanah == "Kering" && cahaya == "Sedang") return "Lama";
  if (suhu == "Sejuk" && tanah == "Kering" && cahaya == "Terik") return "Cepat";

  if (suhu == "Sejuk" && tanah == "Lembab" && cahaya == "Redup") return "Sedang";
  if (suhu == "Sejuk" && tanah == "Lembab" && cahaya == "Sedang") return "Sedang";
  if (suhu == "Sejuk" && tanah == "Lembab" && cahaya == "Terik") return "Cepat";

  if (suhu == "Sejuk" && tanah == "Basah") return "Tidak Disiram";

  if (suhu == "Panas" && tanah == "Kering" && cahaya == "Redup") return "Lama";
  if (suhu == "Panas" && tanah == "Kering" && cahaya == "Sedang") return "Lama";
  if (suhu == "Panas" && tanah == "Kering" && cahaya == "Terik") return "Cepat";

  if (suhu == "Panas" && tanah == "Lembab" && cahaya == "Redup") return "Sedang";
  if (suhu == "Panas" && tanah == "Lembab" && cahaya == "Sedang") return "Sedang";
  if (suhu == "Panas" && tanah == "Lembab" && cahaya == "Terik") return "Cepat";

  if (suhu == "Panas" && tanah == "Basah") return "Tidak Disiram";

  return "Sedang"; // fallback default
}

void loop() {
  float lux = lightMeter.readLightLevel();
  DateTime now = rtc.now();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int sensor_analog = analogRead(sensor_pin);
  int moisture = 100 - ((sensor_analog * 100) / 4095);
  
  String suhuFuzzy = suhuKategori(t);
  String tanahFuzzy = kelembapanTanahKategori(moisture);
  String cahayaFuzzy = cahayaKategori(lux);

  String keputusan = fuzzyKeputusan(suhuFuzzy, tanahFuzzy, cahayaFuzzy);

  Serial.println("====== DATA SENSOR ======");
  Serial.print("Suhu: "); Serial.print(t); Serial.println(" °C (" + suhuFuzzy + ")");
  Serial.print("Kelembapan Tanah: "); Serial.print(moisture); Serial.println(" % (" + tanahFuzzy + ")");
  Serial.print("Cahaya: "); Serial.print(lux); Serial.println(" lux (" + cahayaFuzzy + ")");
  Serial.println("Keputusan: " + keputusan);
  Serial.println("=========================\n");

  // Serial.print("Light: ");
  // Serial.println(lux);
  // Serial.print(" lx | Time: ");
  // Serial.println(now.timestamp());

  // Serial.print("Humidity: ");
  // Serial.print(h);
  // Serial.print(" %\t");
  // Serial.print("Temperature: ");
  // Serial.print(t);
  // Serial.println(" *C");

  // Serial.print("Moisture = ");
  // Serial.print(moisture);
  // Serial.println("%");

  delay(1000);
}