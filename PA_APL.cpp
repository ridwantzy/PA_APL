#include <iostream>
#include <string>
#include "json/json.h"
#include "src/jsoncpp.cpp"   
#include <fstream> 
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>

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
// Prosedur pesan pilihan tidak tersedia
void pilihanTidakTersedia(){
    cout << "\nMenu yang anda pilih tidak tersedia\n";
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

// Fungsi Untuk Menu Admin
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
            case 1: cout << "Manajemen Data Barang\n";
                break;
            case 2: cout << "Manajemen Tukar Uang\n";
                break;
            case 3: cout << "Manajemen Detail Sampah\n";
                break;
            case 4: cout << "Laporan Transaksi\n";
                break;
            case 0:
                return;
            default:
                pilihanTidakTersedia();
        }
    }
}

//Fungsi untuk menu Nasabah
struct WasteType {
    string name;
    string description;
    int pointsPerKg;
};

// Fungsi pembungkus kata
void printWrappedText(const string& text, int startPos, int maxWidth) {
    string word;
    int currentWidth = startPos;
    istringstream words(text);
    
    while (words >> word) {
        if (currentWidth + word.length() > maxWidth) {
            cout << "\n" << setw(startPos) << "" << word << " ";
            currentWidth = startPos + word.length() + 1;
        } else {
            cout << word << " ";
            currentWidth += word.length() + 1;
        }
    }
}

void displayWasteTable() {
    vector<WasteType> wasteTypes = {
        {"Sampah Plastik", "Berbagai jenis plastik rumah tangga termasuk kantong plastik, pembungkus makanan, dan wadah plastik non-botol yang bersih dan kering", 20},
        {"Botol Plastik", "Botol plastik bekas minuman, pembersih, atau kemasan lain yang sudah dibersihkan dan dikeringkan. Sebaiknya dalam kondisi utuh dan tidak pecah", 25},
        {"Kertas/Koran", "Kertas bekas, koran, majalah, buku dalam kondisi bersih dan kering. Tidak menerima kertas yang terkontaminasi minyak atau zat kimia", 15},
        {"Kardus", "Kardus dan karton bekas dalam kondisi bersih, kering, dan sudah dilipat rapi. Tidak menerima kardus yang basah atau terkontaminasi makanan", 25},
        {"Kaleng", "Kaleng aluminium bekas minuman dan makanan yang sudah dibersihkan. Pastikan dalam kondisi tidak berkarat dan tidak mengandung sisa makanan", 30},
        {"Besi", "Berbagai jenis besi bekas seperti besi konstruksi, pagar, atau peralatan rumah tangga berbahan besi yang sudah tidak terpakai", 40},
        {"Aluminium", "Peralatan rumah tangga berbahan aluminium, frame, atau komponen elektronik berbahan aluminium yang sudah tidak terpakai", 70},
        {"Tembaga", "Kabel tembaga bekas, komponen elektronik, atau barang berbahan tembaga lainnya dalam kondisi bersih", 350}
    };

    const int col1Width = 20;
    const int col2Width = 50;
    const int col3Width = 10;
    const int tableWidth = col1Width + col2Width + col3Width + 7; // +7 untuk border dan spacing

    auto printLine = [tableWidth]() {
        cout << "+" << string(tableWidth-2, '-') << "+\n";
    };

    cout << "\nDaftar Jenis Sampah yang Diterima:\n";
    printLine();
    
    cout<< "| " << left << setw(col1Width) << "Jenis Sampah" 
        << "| " << setw(col2Width) << "Deskripsi"
        << "| " << setw(col3Width) << "Poin/kg       |" << endl;
    printLine();

    for (const auto& waste : wasteTypes) {
        cout << "| " << setw(col1Width) << left << waste.name;
        
        // Print deskripsi dengan word wrap
        istringstream desc(waste.description);
        vector<string> words;
        string word;
        string line;
        bool firstLine = true;
        
        while (desc >> word) {
            if (line.length() + word.length() + 1 <= col2Width - 2) {
                if (!line.empty()) line += " ";
                line += word;
            } else {
                if (firstLine) {
                    cout<< "| " << setw(col2Width) << left << line
                        << "| " << setw(col3Width) << right << waste.pointsPerKg << " |" << endl;
                    firstLine = false;
                } else {
                    cout<< "| " << setw(col1Width) << "" 
                        << "| " << setw(col2Width) << left << line
                        << "| " << setw(col3Width) << "" << " |" << endl;
                }
                line = word;
            }
        }

        if (!line.empty()) {
            if (firstLine) {
                cout<< "| " << setw(col2Width) << left << line
                    << "| " << setw(col3Width) << left << waste.pointsPerKg << " |" << endl;
            } else {
                cout << "| " << setw(col1Width) << "" 
                    << "| " << setw(col2Width) << left << line
                    << "| " << setw(col3Width) << "" << " |" << endl;
            }

        }
        printLine();
    }

    cout << "\nKeterangan:\n";
    cout << "- Semua sampah harus dalam kondisi bersih dan kering\n";
    cout << "- Poin dapat ditukarkan dengan berbagai hadiah menarik\n";
    cout << "- Sampah dalam kondisi basah atau kotor tidak akan diterima\n";
}

// Fungsi untuk menghapus spasi input pencarian
string hapusSpasi(const string& teks){
    string hasil;
    for (char huruf : teks){
        if (huruf != ' '){
            hasil += tolower(huruf);
        };
    }
    return hasil;
}

// Fungsi untuk pencarian Bank Sampah
void CariBankSampah(){
    string kataKunci;
    cout << "\nMasukkan kata kunci (nama Bank atau alamat): ";
    cin.ignore();
    getline(cin, kataKunci);

    string kataKunciNormal = hapusSpasi(kataKunci);
    bool ditemukan = false;
    
    cout << "\nHasil pencarian untuk '" << kataKunci << "':" << endl;
    cout << "----------------------------------------" << endl;

    for (const auto& lokasi : database["lokasi_bank_sampah"]){
        string namaLokasi = lokasi["nama"].asString();
        string alamatLokasi = lokasi["alamat"].asString();
        string gabungan = namaLokasi + " " + alamatLokasi;
        string gabunganNormal = hapusSpasi(gabungan);

        if (gabunganNormal.find(kataKunciNormal) != string::npos){
            cout << "- Nama     : " << namaLokasi << endl;
            cout << "  Alamat   : " << alamatLokasi << endl;
            cout << "----------------------------------------" << endl;
            ditemukan = true;
        }
    }

    if (!ditemukan){
        cout << "Lokasi dengan kata kunci '" << kataKunci << "' tidak ditemukan.\n";
        cout << "----------------------------------------" << endl;
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
                displayWasteTable();
                break;
            case 2:
                CariBankSampah();
                break;
            case 3:
                cout << "Riwayat Transaksi:\n";
                for (const auto& trx : database["transactions"]) {
                    if (trx["username"].asString() == currentUser.username) {
                        cout<< "- ID: " << trx["id_transaksi"].asString()
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
                pilihanTidakTersedia();
        }
    }
}

// Menu Petugas
// Fungsi untuk menu Petugas
void petugasMenu() {
    int choice;
    while (true) {
        cout << "\n=== Menu Petugas ===\n";
        cout << "1. Input Detail Sampah" << endl;
        cout << "2. Lihat Catatan Transaksi" << endl;
        cout << "3. Informasi Jenis Sampah" << endl;
        cout << "0. Keluar\n" << endl;
        cout << "Pilihan: ";
        cin >> choice;

        switch (choice) {
            case 1: 
                inputTransaksiPetugas();
                break;
            case 2:
                cout << "Lihat Catatan Transaksi\n";
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
            case 3:
                cout << "Informasi Jenis Sampah\n"; break;
            case 0:
                return;
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
    cout << "Login gagal!!\nUsername atau password salah.\n";
}

// Fungsi Main
int main() {
    cout << "Bak Sampah Digital 'Greencycle Program'" << endl;

    // Muat database dari file JSON
    loadDatabase();

    while (true) {
        int choice;
        cout << "\n=== Menu Utama ===\n";
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "0. Logout\n" << endl;
        cout << "Pilihan : ";
        cin >> choice;

        switch (choice) {
            case 1:
                login();
                break;
            case 2:
                registerUser();
                break;
            case 0:
                logout();
                return 0;
            default:
                pilihanTidakTersedia();
        }
    }

    return 0;
}