#include <iostream>
#include <string>
#include "json/json.h"// Header jsoncpp
#include "src/jsoncpp.cpp"   // Untuk operasi file
#include <fstream> 

using namespace std;

// Struktur data untuk pengguna
struct User {
    string username;
    string password;
    string role;
    int poin = 0;
};
User currentUser;

Json::Value database;

// Fungsi untuk membaca data dari file JSON
Json::Value readJSON(const string& filename) {
    ifstream file(filename, ifstream::binary);
    if (!file.is_open()) {
        cerr << "Gagal membuka file: " << filename << endl;
        exit(1);
    }
    Json::Value data;
    file >> data;
    file.close();
    return data;
}

// Fungsi untuk menulis data ke file JSON
void writeJSON(const string& filename, const Json::Value& data) {
    ofstream file(filename, ofstream::binary);
    if (!file.is_open()) {
        cerr << "Gagal membuka file: " << filename << endl;
        exit(1);
    }
    file << data.toStyledString(); // Format JSON dengan indentasi
    file.close();
}

// Fungsi untuk memuat data dari file JSON
void loadDatabase() {
    database = readJSON("database.json");
}

// Fungsi untuk menyimpan data ke file JSON
void saveDatabase() {
    writeJSON("database.json", database);
}
// Fungsi untuk input transaksi oleh petugas
void inputTransaksiPetugas() {
    string username, jenisSampah;
    double beratKg;

    cout << "Masukkan username nasabah: ";
    cin >> username;

    // Periksa apakah username nasabah ada di database
    bool userDitemukan = false;
    for (const auto& user : database["users"]) {
        if (user["username"].asString() == username && user["role"].asString() == "nasabah") {
            userDitemukan = true;
            break;
        }
    }

    if (!userDitemukan) {
        cout << "Nasabah dengan username tersebut tidak ditemukan.\n";
        return;
    }

    cout << "Masukkan jenis sampah (contoh: Plastik, Kertas, Logam): ";
    cin.ignore();
    getline(cin, jenisSampah);
    cout << "Masukkan berat sampah (kg): ";
    cin >> beratKg;

    // Cari jenis sampah di database
    int poinPerKg = 0;
    bool jenisDitemukan = false;
    for (const auto& item : database["sampah"]) {
        if (item["nama"].asString() == jenisSampah) {
            poinPerKg = item["poin_per_kg"].asInt();
            jenisDitemukan = true;
            break;
        }
    }

    if (!jenisDitemukan) {
        cout << "Jenis sampah tidak ditemukan dalam database.\n";
        return;
    }

    // Hitung poin yang diterima
    int poinDiterima = static_cast<int>(beratKg * poinPerKg);

    // Buat transaksi baru
    Json::Value transaksiBaru;
    transaksiBaru["id_transaksi"] = "TRX" + to_string(database["transactions"].size() + 1);
    transaksiBaru["jenis_sampah"] = jenisSampah;
    transaksiBaru["berat_kg"] = beratKg;
    transaksiBaru["poin_diterima"] = poinDiterima;
    transaksiBaru["tanggal"] = "2025-05-11"; // Tanggal bisa diubah sesuai kebutuhan
    transaksiBaru["username"] = username;

    // Tambahkan transaksi ke database
    database["transactions"].append(transaksiBaru);

    // Tambahkan poin ke nasabah
    for (auto& user : database["users"]) {
        if (user["username"].asString() == username) {
            user["poin"] = user["poin"].asInt() + poinDiterima;
            break;
        }
    }

    saveDatabase(); // Simpan perubahan ke file JSON
    cout << "Transaksi berhasil ditambahkan. Nasabah mendapatkan " << poinDiterima << " poin.\n";
}

// Fungsi Register
void registerUser() {
    string username, password;
    cout << "Masukkan username baru: "; cin >> username;
    cout << "Masukkan password baru: "; cin >> password;

    Json::Value newUser;
    newUser["username"] = username;
    newUser["password"] = password;
    newUser["role"] = "nasabah";
    newUser["poin"] = 0;

    database["users"].append(newUser);
    saveDatabase(); // Simpan perubahan ke file JSON
    cout << "Registrasi berhasil. Silakan login.\n";
}

// Fungsi Logout
void logout() {
    currentUser = {};
    cout << "Anda telah logout.\n";
}

// Menu Admin
void adminMenu() {
    int choice;
    while (true) {
        cout << "\n=== Menu Admin ===\n";
        cout << "1. Manajemen Data Barang\n";
        cout << "2. Manajemen Tukar Uang\n";
        cout << "3. Manajemen Detail Sampah\n";
        cout << "4. Laporan Transaksi\n";
        cout << "0. Keluar\nPilih: ";
        cin >> choice;

        switch (choice) {
            case 1: cout << "Manajemen Data Barang\n"; break;
            case 2: cout << "Manajemen Tukar Uang\n"; break;
            case 3: cout << "Manajemen Detail Sampah\n"; break;
            case 4: cout << "Laporan Transaksi\n"; break;
            case 0: return;
            default: cout << "Pilihan salah.\n";
        }
    }
}

// Menu Nasabah
void nasabahMenu() {
    int choice;
    while (true) {
        cout << "\n=== Menu Nasabah ===\n";
        cout << "1. Melihat Jenis Sampah\n";
        cout << "2. Mencari Lokasi Bank Sampah\n";
        cout << "3. Melihat Riwayat Transaksi\n";
        cout << "4. Menukar Poin\n";
        cout << "5. Tentang Program\n";
        cout << "0. Keluar\nPilih: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Jenis Sampah:\n";
                for (const auto& item : database["sampah"]) {
                    cout << "- " << item["nama"].asString() << " (" << item["kategori"].asString() << ") | Poin/kg: " << item["poin_per_kg"].asInt() << endl;
                }
                break;
            case 2:
                cout << "Lokasi Bank Sampah:\n";
                for (const auto& loc : database["lokasi_bank_sampah"]) {
                    cout << "- " << loc["nama"].asString() << "\n  Alamat: " << loc["alamat"].asString() << endl;
                }
                break;
            case 3:
                cout << "Riwayat Transaksi:\n";
                for (const auto& trx : database["transactions"]) {
                    if (trx["username"].asString() == currentUser.username) {
                        cout << "- ID: " << trx["id_transaksi"].asString()
                             << ", Jenis: " << trx["jenis_sampah"].asString()
                             << ", Berat: " << trx["berat_kg"].asDouble() << "kg"
                             << ", Poin: " << trx["poin_diterima"].asInt()
                             << ", Tanggal: " << trx["tanggal"].asString() << endl;
                    }
                }
                break;
            case 4:
                cout << "Poin Anda: " << currentUser.poin << "\n";
                // Tambahkan logika untuk menukar poin
                break;
            case 5:
                cout << "Program Greencycle adalah platform digital untuk mengelola bank sampah secara efektif dan ramah lingkungan.\n";
                break;
            case 0:
                return;
            default:
                cout << "Pilihan salah.\n";
        }
    }
}

// Menu Petugas
void petugasMenu() {
    int choice;
    while (true) {
        cout << "\n=== Menu Petugas ===\n";
        cout << "1. Input Detail Sampah\n";
        cout << "2. Lihat Catatan Transaksi\n";
        cout << "3. Informasi Jenis Sampah\n";
        cout << "0. Keluar\nPilih: ";
        cin >> choice;

        switch (choice) {
            case 1: 
                inputTransaksiPetugas();
                break;
            case 2: cout << "Lihat Catatan Transaksi\n";
                cout << "Catatan Transaksi:\n";
                for (const auto& trx : database["transactions"]) {
                    cout << "- ID: " << trx["id_transaksi"].asString()
                         << ", Jenis: " << trx["jenis_sampah"].asString()
                         << ", Berat: " << trx["berat_kg"].asDouble() << "kg"
                         << ", Poin: " << trx["poin_diterima"].asInt()
                         << ", Tanggal: " << trx["tanggal"].asString()
                         << ", Username: " << trx["username"].asString() << endl;
                }
                break;
            case 3: cout << "Informasi Jenis Sampah\n"; break;
            case 0: return;
            default: cout << "Pilihan salah.\n";
        }
    }
}


// Fungsi Login
void login() {
    string username, password;
    cout << "Username: "; cin >> username;
    cout << "Password: "; cin >> password;

    for (const auto& user : database["users"]) {
        if (user["username"].asString() == username && user["password"].asString() == password) {
            currentUser.username = user["username"].asString();
            currentUser.password = user["password"].asString();
            currentUser.role = user["role"].asString();
            currentUser.poin = user.get("poin", 0).asInt(); // Gunakan default poin = 0 jika tidak ada
            cout << "Login berhasil sebagai " << currentUser.role << ".\n";

            if (currentUser.role == "admin") {
                adminMenu();
            } else if (currentUser.role == "nasabah") {
                nasabahMenu();
            } else if (currentUser.role == "petugas") {
                petugasMenu();
            }
            return;
        }
    }
    cout << "Login gagal. Username atau password salah.\n";
}

// Fungsi Main
int main() {
    cout << "Bak Sampah Digital 'Greencycle Program'" << endl;

    // Muat database dari file JSON
    loadDatabase();

    while (true) {
        int choice;
        cout << "\n=== Menu Utama ===\n";
        cout << "1. Login\n2. Register\n3. Logout\n0. Keluar\nPilih: ";
        cin >> choice;

        switch (choice) {
            case 1: login(); break;
            case 2: registerUser(); break;
            case 3: logout(); break;
            case 0: return 0;
            default: cout << "Pilihan salah.\n";
        }
    }

    return 0;
}