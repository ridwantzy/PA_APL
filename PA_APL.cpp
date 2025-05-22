#include <iostream>
#include <string>
#include "json/json.h"
#include "src/jsoncpp.cpp"   
#include <fstream> 
#include <iomanip>
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

// Fungsi Register
void registerUser() {
    string username, password;
    cout << "\n" << endl;
    cout << "╔════════════════════════════════════════════╗\n";
    cout << "║             REGISTRASI AKUN                ║\n";
    cout << "╠════════════════════════════════════════════╣\n";
    cout << "   Username: ";
    cin >> username;
    cout << "╠════════════════════════════════════════════╣\n";
    cout << "   Password: ";
    cin >> password;
    cout << "╠════════════════════════════════════════════╣\n";

    Json::Value newUser;
    newUser["username"] = username;
    newUser["password"] = password;
    newUser["role"] = "nasabah";
    newUser["poin"] = 0;

    database["users"].append(newUser);
    saveDatabase();
    cout << "╔════════════════════════════════════════════╗\n";
    cout << "║    Registrasi Berhasil, sialhakn Login!    ║\n";
    cout << "╠════════════════════════════════════════════╣\n";
    cout << "\nRegistrasi berhasil. Silakan login.\n";
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
        cout << "\n" << endl;
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║              MENU ADMIN                ║\n";
        cout << "╠════════════════════════════════════════╣\n";
        cout << "║  1. Manajemen Data Barang              ║\n";
        cout << "║  2. Manajemen Tukar Uang               ║\n";
        cout << "║  3. Manajemen Detail Sampah            ║\n";
        cout << "║  4. Laporan Transaksi                  ║\n";
        cout << "║  0. Keluar                             ║\n";
        cout << "╠════════════════════════════════════════╣\n";
        cout << "   Pilih menu: ";
        cin >> choice;
        cout << "╚════════════════════════════════════════╝\n";

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
    if (database.isNull() || !database.isMember("sampah")) {
        cout << "\n[KESALAHAN] Data sampah tidak tersedia atau database belum dimuat." << endl;
        cout << "Silakan hubungi Admin untuk pengecekan." << endl;
        return;
    }

    const Json::Value& daftarSampah = database["sampah"];

    if (daftarSampah.empty()) {
        cout << "\nBelum ada data jenis sampah yang terdaftar dalam sistem." << endl;
        cout << "Informasi akan segera diperbarui." << endl;
        return;
    }


    const int noColWidth = 5;        
    const int jenisColWidth = 20;   
    const int descColWidth = 75;     
    const int poinColWidth = 10;     

    cout << "\nDAFTAR JENIS SAMPAH YANG DITERIMA" << endl;

    // Baris atas tabel
    cout << "╔══════╦═════════════════════╦════════════════════════════════════════════════════════════════════════════╦════════════╗\n";
    
    // Header kolom
    cout << "║ " << left << setw(noColWidth) << "No"
         << "║ " << left << setw(jenisColWidth) << "Jenis Sampah"
         << "║ " << left << setw(descColWidth) << "Deskripsi"
         << "║ " << right << setw(poinColWidth) << "Poin" << " ║\n";

    // Garis pemisah header
    cout << "╠══════╬═════════════════════╬════════════════════════════════════════════════════════════════════════════╬════════════╣\n";

    int nomor = 1;
    for (const auto& itemSampah : daftarSampah) {
        string nama = itemSampah.get("nama", "N/A").asString();
        string deskripsi = itemSampah.get("deskripsi", "N/A").asString();
        int poin = itemSampah.get("poin_per_kg", 0).asInt();

        // Potong dan format nama
        string namaTampil = nama;
        if (namaTampil.length() > jenisColWidth) {
            namaTampil = namaTampil.substr(0, jenisColWidth - 3) + "...";
        }

        // Potong dan format deskripsi
        string deskripsiTampil = deskripsi;
        if (deskripsiTampil.length() > descColWidth) {
            deskripsiTampil = deskripsiTampil.substr(0, descColWidth - 3) + "...";
        }

        // Cetak baris data dengan format yang tepat
        cout << "║ " << right << setw(3) << nomor << "  "  // Format nomor dengan lebar 3 + 2 spasi
             << "║ " << left << setw(jenisColWidth) << namaTampil 
             << "║ " << left << setw(descColWidth) << deskripsiTampil
             << "║ " << right << setw(poinColWidth) << poin << " ║\n";

        nomor++;
    }

    // Baris bawah tabel
    cout << "╚══════╩═════════════════════╩════════════════════════════════════════════════════════════════════════════╩════════════╝\n";

    // Keterangan Tambahan
    cout << "\nKeterangan Penting:\n";
    cout << "1. Pastikan sampah yang Anda setorkan dalam kondisi BERSIH dan KERING.\n";
    cout << "2. Pemilahan sampah berdasarkan jenis akan mempercepat proses.\n";
    cout << "3. Poin yang terkumpul dapat ditukarkan dengan uang atau barang (jika tersedia).\n";
    cout << "4. Petugas berhak menolak sampah yang tidak memenuhi syarat.\n";
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
void CariBankSampah() {
    if (database.isNull() || !database.isMember("lokasi_bank_sampah")) {
        cout << "\n[KESALAHAN] Data lokasi bank sampah tidak tersedia.\n";
        return;
    }

    // Kumpulkan daftar provinsi unik dari database
    set<string> daftarProvinsi;
    for (const auto& lokasi : database["lokasi_bank_sampah"]) {
        string provinsi = lokasi.get("Provinsi", "").asString();
        if (!provinsi.empty()) {
            daftarProvinsi.insert(provinsi);
        }
    }

    // Tampilkan daftar provinsi yang tersedia
    cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
    cout << "║                DAFTAR PROVINSI YANG TERSEDIA              ║\n";
    cout << "╠═══════════════════════════════════════════════════════════╣\n";
    
    int nomor = 1;
    for (const auto& provinsi : daftarProvinsi) {
        cout << "║  " << nomor << ". " << left << setw(54) << provinsi << "║\n";
        nomor++;
    }
    
    cout << "╚═══════════════════════════════════════════════════════════╝\n";

    // ...rest of the existing search code...
    string kataKunci;
    cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
    cout << "║                     PENCARIAN LOKASI                      ║\n";
    cout << "╠═══════════════════════════════════════════════════════════╣\n";
    cout << "║  Masukkan kata kunci (nama/alamat/provinsi):              ║\n";
    cout << "╚═══════════════════════════════════════════════════════════╝\n";
    cout << "→ ";
    cin.ignore();
    getline(cin, kataKunci);

    string kataKunciNormal = hapusSpasi(kataKunci);
    bool ditemukan = false;
    
    cout << "\n╔═══════════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║HASIL PENCARIAN: " << left << setw(70) << kataKunci << "║\n";
    cout << "╠═══════════════════════════════════════════════════════════════════════════════════════╣\n";

    for (const auto& lokasi : database["lokasi_bank_sampah"]) {
        string namaLokasi = lokasi["nama"].asString();
        string alamatLokasi = lokasi["alamat"].asString();
        string provinsiLokasi = lokasi.get("Provinsi", "").asString();
        if (provinsiLokasi.empty()) {
            provinsiLokasi = lokasi.get("Privinsi", "").asString(); // Handle typo in JSON
        }
        
        string gabungan = namaLokasi + " " + alamatLokasi + " " + provinsiLokasi;
        string gabunganNormal = hapusSpasi(gabungan);

        if (gabunganNormal.find(kataKunciNormal) != string::npos) {
            cout << "║  Nama     : " << left << setw(74) << namaLokasi << "║\n";
            cout << "║  Alamat   : " << left << setw(74) << alamatLokasi << "║\n";
            if (!provinsiLokasi.empty()) {
                cout << "║  Provinsi : " << left << setw(74) << provinsiLokasi << "║\n";
            }
            cout << "╠═══════════════════════════════════════════════════════════════════════════════════════╣\n";
            ditemukan = true;
        }
    }

    if (!ditemukan) {
        cout << "║  Lokasi tidak ditemukan                                                               ║\n";
        cout << "╚═══════════════════════════════════════════════════════════════════════════════════════╝\n";
    } else {
        cout << "╚═══════════════════════════════════════════════════════════════════════════════════════╝\n";
    }
}

void riwayatTransaksi(){
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
    cout << std::string(75, '=') << endl;
}

void tentangProgram() {
    cout << "\n╔═════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                     TENTANG GREENCYCLE PROGRAM                      ║\n";
    cout << "╠═════════════════════════════════════════════════════════════════════╣\n";
    cout << "║  Versi     : 1.0.0                                                  ║\n";
    cout << "║  Rilis     : Mei 2024                                               ║\n";
    cout << "╠═════════════════════════════════════════════════════════════════════╣\n";
    cout << "║                           DESKRIPSI                                 ║\n";
    cout << "║  Greencycle Program adalah sistem manajemen bank sampah digital     ║\n";
    cout << "║  yang dirancang untuk memudahkan pengelolaan sampah dan             ║\n";
    cout << "║  mendorong partisipasi masyarakat dalam program daur ulang.         ║\n";
    cout << "║  Program ini memungkinkan pengguna untuk menukar sampah dengan      ║\n";
    cout << "║  poin yang dapat dikonversikan menjadi uang atau barang.            ║\n";
    cout << "╠═════════════════════════════════════════════════════════════════════╣\n";
    cout << "║                        DIKEMBANGKAN OLEH                            ║\n";
    cout << "║                           KELOMPOK 6                                ║\n";
    cout << "║                                                                     ║\n";
    cout << "║  • Rahmat Riyadi            (2409106074)                            ║\n";
    cout << "║  • Muhammad Ridwanansyah R  (2409106073)                            ║\n";
    cout << "║  • Razib Ramadhan           (2409106076)                            ║\n";
    cout << "╠═════════════════════════════════════════════════════════════════════╣\n";
    cout << "║            Praktikum Algoritma dan Pemrograman Lanjut               ║\n";
    cout << "║              Informatika, Universitas Mulawarman                    ║\n";
    cout << "╚═════════════════════════════════════════════════════════════════════╝\n";
}

void menukarPoin(){
    cout << "\n=== Menu Tukar Poin ===\n";
    cout << "Poin Anda saat ini: " << currentUser.poin << endl;
    cout << "1. Tukar dengan Uang\n";
    cout << "2. Tukar dengan Barang\n";
    cout << "0. Kembali\n";
    cout << "Pilih: ";
    int pilihTukar;
    cin >> pilihTukar;

    if (pilihTukar == 1) {
        int jumlahTukar;
        cout << "Masukkan jumlah poin yang ingin ditukar: ";
        cin >> jumlahTukar;
        if (jumlahTukar > 0 && jumlahTukar <= currentUser.poin) {
            string norek, bank;
            cout << "Masukkan nomor rekening tujuan: ";
            cin.ignore(); // flush newline
            getline(cin, norek);
            cout << "Masukkan nama bank: ";
            getline(cin, bank);

            int uang = jumlahTukar * 10;
            currentUser.poin -= jumlahTukar;
            // Update database
            for (auto& user : database["users"]) {
                if (user["username"].asString() == currentUser.username) {
                    user["poin"] = currentUser.poin;
                    break;
                }
            }
            saveDatabase();
            cout << "\nPertukaran berhasil!\n";
            cout << "Anda mendapatkan uang sebesar Rp" << uang << ".\n";
            cout << "Transfer ke rekening: " << norek << " (" << bank << ")\n";
        } else {
            cout << "Jumlah poin tidak valid.\n";
        }
    } else if (pilihTukar == 2) {
        cout << "\nDaftar Barang yang Bisa Ditukar:\n";
        cout << left << setw(5) << "No" << setw(20) << "Nama Barang" << setw(10) << "Harga(Poin)" << endl;
        cout << std::string(35, '-') << endl;
        int idx = 1;
        vector<pair<string, int>> daftarBarang;
        for (const auto& barang : database["barang_tukar"]) {
            cout << setw(5) << idx << setw(20) << barang["nama"].asString() << setw(10) << barang["harga_poin"].asInt() << endl;
            daftarBarang.push_back({barang["nama"].asString(), barang["harga_poin"].asInt()});
            idx++;
        }
        cout << "Pilih nomor barang yang ingin ditukar (0 untuk batal): ";
        int pilihBarang;
        cin >> pilihBarang;
        if (pilihBarang > 0 && pilihBarang <= daftarBarang.size()) {
            int harga = daftarBarang[pilihBarang-1].second;
            if (currentUser.poin >= harga) {
                currentUser.poin -= harga;
                // Update database
                for (auto& user : database["users"]) {
                    if (user["username"].asString() == currentUser.username) {
                        user["poin"] = currentUser.poin;
                        break;
                    }
                }
                saveDatabase();
                cout << "Pertukaran berhasil! Anda mendapatkan barang: " << daftarBarang[pilihBarang-1].first << endl;
            } else {
                cout << "Poin Anda tidak cukup untuk menukar barang ini.\n";
            }
        } else if (pilihBarang == 0) {
            cout << "Batal menukar barang.\n";
        } else {
            cout << "Pilihan tidak valid.\n";
        }
    } else if (pilihTukar == 0) {
        cout << "Kembali ke menu nasabah.\n";
    } else {
        cout << "Pilihan tidak valid.\n";
    }
}

// Menu Nasabah
void nasabahMenu() {
    int choice;
    while (true) {
        cout << "\n" << endl;
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║            MENU NASABAH                ║\n";
        cout << "╠════════════════════════════════════════╣\n";
        cout << "║  1. Melihat Jenis Sampah               ║\n";
        cout << "║  2. Mencari Lokasi Bank Sampah         ║\n";
        cout << "║  3. Melihat Riwayat Transaksi          ║\n";
        cout << "║  4. Menukar Poin                       ║\n";
        cout << "║  5. Tentang Program                    ║\n";
        cout << "║  0. Keluar                             ║\n";
        cout << "╠════════════════════════════════════════╣\n";
        cout << "   Pilih menu: ";
        cin >> choice;
        cout << "╚════════════════════════════════════════╝\n";

        switch (choice) {
            case 1:
                displayWasteTable();
                break;
            case 2:
                CariBankSampah();
                break;
            case 3:
                riwayatTransaksi();
                break;
            case 4: 
                menukarPoin();
                break;
            case 5:
                tentangProgram();
                break;
            case 0:
                return;
            default:
                pilihanTidakTersedia();
        }
    }
}

// Menu Petugas
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

// Fungsi untuk melihat catatan transaksi
void lihatCatatanTransaksi() {
    cout << "Lihat Catatan Transaksi\n";
    cout << "Catatan Transaksi:\n";
    for (const auto& trx : database["transactions"]) {
        cout << "- ID: " << trx["id_transaksi"].asString()
            << ", Username: " << trx["username"].asString()
            << ", Tanggal: " << trx["tanggal"].asString()
            << ", Jenis: " << trx["jenis_sampah"].asString()
            << ", Berat: " << trx["berat_kg"].asDouble() << "kg"
            << ", Poin: " << trx["poin_diterima"].asInt() << endl;
    }
}
// Fungsi untuk menu Petugas
void petugasMenu() {
    int choice;
    while (true) {
        cout << "\n" << endl;
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║             MENU PETUGAS               ║\n";
        cout << "╠════════════════════════════════════════╣\n";
        cout << "║  1. Input Detail Sampah                ║\n";
        cout << "║  2. Lihat Catatan Transaksi            ║\n";
        cout << "║  3. Informasi Jenis Sampah             ║\n";
        cout << "║  0. Keluar                             ║\n";
        cout << "╠════════════════════════════════════════╣\n";
        cout << "   Pilih menu: ";
        cin >> choice;
        cout << "╚════════════════════════════════════════╝\n";

        switch (choice) {
            case 1: 
                inputTransaksiPetugas();
                break;
            case 2:
                lihatCatatanTransaksi();
                break;
            case 3:
                displayWasteTable(); 
                break;
            case 0:
                return;
            default: cout << "Pilihan salah.\n";
        }
    }
}

// Fungsi Login
void login() {
    string username, password;
    cout << "╔════════════════════════════════════════════╗\n";
    cout << "║               LOGIN SISTEM                 ║\n";
    cout << "╠════════════════════════════════════════════╣\n";
    cout << "   Username: ";
    cin >> username;
    cout << "╠════════════════════════════════════════════╣\n";
    cout << "   Password: ";
    cin >> password;

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
        cout << "\n" << endl;
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║             MENU UTAMA                 ║\n";
        cout << "╠════════════════════════════════════════╣\n";
        cout << "║  1. Login                              ║\n";
        cout << "║  2. Register                           ║\n";
        cout << "║  0. Logout                             ║\n";
        cout << "╠════════════════════════════════════════╣\n";
        cout << "   Pilih menu: ";
        cin >> choice;
        cout << "╚════════════════════════════════════════╝\n";

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