#include <iostream>
#include <algorithm>
using namespace std;

//FUZZIFIKASI
// ======== SUHU ========
float suhu_dingin(float x) {
    if (x <= 20) return 1;
    else if (x > 20 && x < 25) return (25 - x) / 5.0;
    else return 0;
}

float suhu_normal(float x) {
    if (x <= 20 || x >= 30) return 0;
    else if (x > 20 && x < 25) return (x - 20) / 5.0;
    else if (x >= 25 && x < 30) return (30 - x) / 5.0;
    else return 0;
}

float suhu_panas(float x) {
    if (x <= 25) return 0;
    else if (x > 25 && x < 30) return (x - 25) / 5.0;
    else return 1;
}


// ======== KELEMBAPAN TANAH ========
float lembap_kering(float x) {
    if (x <= 30) return 1;
    else if (x > 30 && x < 50) return (50 - x) / 20.0;
    else return 0;
}

float lembap_sedang(float x) {
    if (x <= 30 || x >= 70) return 0;
    else if (x > 30 && x < 50) return (x - 30) / 20.0;
    else if (x >= 50 && x < 70) return (70 - x) / 20.0;
    else return 0;
}

float lembap_basah(float x) {
    if (x <= 50) return 0;
    else if (x > 50 && x < 70) return (x - 50) / 20.0;
    else return 1;
}

// ======== INTENSITAS CAHAYA ========
float cahaya_redup(float x) {
    if (x <= 300) return 1;
    else if (x > 300 && x < 500) return (500 - x) / 200.0;
    else return 0;
}

float cahaya_sedang(float x) {
    if (x <= 300 || x >= 700) return 0;
    else if (x > 300 && x < 500) return (x - 300) / 200.0;
    else if (x >= 500 && x < 700) return (700 - x) / 200.0;
    else return 0;
}

float cahaya_terang(float x) {
    if (x <= 500) return 0;
    else if (x > 500 && x < 700) return (x - 500) / 200.0;
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

int main(){
    float suhu, kelembapan, cahaya;

    // === Input ===
    cout << "Masukkan suhu (C): ";
    cin >> suhu;
    cout << "Masukkan kelembapan tanah (%): ";
    cin >> kelembapan;
    cout << "Masukkan intensitas cahaya (lux): ";
    cin >> cahaya;

    // === Fuzzifikasi ===
    float mu_suhu_dingin = suhu_dingin(suhu);
    float mu_suhu_sejuk = suhu_normal(suhu);
    float mu_suhu_panas = suhu_panas(suhu);

    float mu_lembap_kering = lembap_kering(kelembapan);
    float mu_lembap_lembab = lembap_sedang(kelembapan);
    float mu_lembap_basah  = lembap_basah(kelembapan);

    float mu_cahaya_redup  = cahaya_redup(cahaya);
    float mu_cahaya_sedang = cahaya_sedang(cahaya);
    float mu_cahaya_terik = cahaya_terang(cahaya);

    // === Inferensi ===
    // float rule1 = fuzzy_and(fuzzy_and(mu_suhu_panas, mu_lembap_kering), mu_cahaya_terang);
    // float rule2 = fuzzy_and(fuzzy_and(mu_suhu_normal, mu_lembap_sedang), mu_cahaya_sedang);
    // float rule3 = fuzzy_or(mu_suhu_dingin, mu_lembap_basah);
    // Aturan (27 rule)
    float r[27];
    int i = 0;

    // Suhu Dingin
    r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_kering, mu_cahaya_redup);  // Lama
    r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_kering, mu_cahaya_sedang); // Sedang
    r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_kering, mu_cahaya_terik);  // Cepat
    r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_lembab, mu_cahaya_redup);  // Tidak
    r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_lembab, mu_cahaya_sedang); // Tidak
    r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_lembab, mu_cahaya_terik);  // Tidak
    r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_basah, mu_cahaya_redup);   // Tidak
    r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_basah, mu_cahaya_sedang);  // Tidak
    r[i++] = fuzzy_and3(mu_suhu_dingin, mu_lembap_basah, mu_cahaya_terik);   // Tidak

    // Suhu Sejuk
    r[i++] = fuzzy_and3(mu_suhu_sejuk, mu_lembap_kering, mu_cahaya_redup);   // Lama
    r[i++] = fuzzy_and3(mu_suhu_sejuk, mu_lembap_kering, mu_cahaya_sedang);  // Lama
    r[i++] = fuzzy_and3(mu_suhu_sejuk, mu_lembap_kering, mu_cahaya_terik);   // Cepat
    r[i++] = fuzzy_and3(mu_suhu_sejuk, mu_lembap_lembab, mu_cahaya_redup);   // Sedang
    r[i++] = fuzzy_and3(mu_suhu_sejuk, mu_lembap_lembab, mu_cahaya_sedang);  // Sedang
    r[i++] = fuzzy_and3(mu_suhu_sejuk, mu_lembap_lembab, mu_cahaya_terik);   // Cepat
    r[i++] = fuzzy_and3(mu_suhu_sejuk, mu_lembap_basah, mu_cahaya_redup);    // Tidak
    r[i++] = fuzzy_and3(mu_suhu_sejuk, mu_lembap_basah, mu_cahaya_sedang);   // Tidak
    r[i++] = fuzzy_and3(mu_suhu_sejuk, mu_lembap_basah, mu_cahaya_terik);    // Tidak

    // Suhu Panas
    r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_kering, mu_cahaya_redup);   // Lama
    r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_kering, mu_cahaya_sedang);  // Lama
    r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_kering, mu_cahaya_terik);   // Cepat
    r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_lembab, mu_cahaya_redup);   // Sedang
    r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_lembab, mu_cahaya_sedang);  // Sedang
    r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_lembab, mu_cahaya_terik);   // Cepat
    r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_basah, mu_cahaya_redup);    // Tidak
    r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_basah, mu_cahaya_sedang);   // Tidak
    r[i++] = fuzzy_and3(mu_suhu_panas, mu_lembap_basah, mu_cahaya_terik);    // Tidak

    // Output fuzzy untuk masing-masing aturan
    // float mu_penyiraman_rendah = rule3;
    // float mu_penyiraman_sedang = rule2;
    // float mu_penyiraman_tinggi = rule1;
    // Gabungkan berdasarkan output
    float mu_tidak = max({r[3], r[4], r[5], r[6], r[7], r[8], r[15], r[16], r[17], r[24], r[25], r[26]});
    float mu_sedang = max({r[1], r[13], r[14], r[21], r[22]});
    float mu_cepat  = max({r[2], r[11], r[14], r[20], r[23]});
    float mu_lama   = max({r[0], r[9], r[10], r[18], r[19]});

    // === Output hasil inferensi ===
    cout << "\n--- Hasil Inferensi ---\n";
    cout << "Penyiraman Tidak : " << mu_tidak << endl;
    cout << "Penyiraman Rendah : " << mu_sedang << endl;
    cout << "Penyiraman Sedang : " << mu_cepat << endl;
    cout << "Penyiraman Tinggi : " << mu_lama << endl;

    // === Defuzzifikasi ===
    // Misal: representasi crisp untuk penyiraman
    // float z_rendah = 30;
    // float z_sedang = 60;
    // float z_tinggi = 90;
    // Nilai crisp output dalam detik
    float z_tidak = 0;
    float z_sedang = 10;
    float z_cepat = 15;
    float z_lama = 20;

    float numerator = (mu_tidak * z_tidak) +
                      (mu_sedang * z_sedang) +
                      (mu_cepat * z_cepat)+
                      (mu_lama * z_lama);

    float denominator = mu_tidak + mu_sedang + mu_cepat + mu_lama;

    float z_output = (denominator == 0) ? 0 : numerator / denominator;

    cout << "\n>>> Nilai akhir penyiraman (crisp): " << z_output << " (skala 0–100)" << endl;

    return 0;
}