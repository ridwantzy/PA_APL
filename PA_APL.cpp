#include <iostream>
#include <string>
#include "json/json.h"
#include "src/jsoncpp.cpp"
#include <windows.h>   
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

bool isValidRekening(const string& norek) {
    // Cek panjang minimal dan maksimal
    if (norek.length() < 10 || norek.length() > 16) {
        return false;
    }
    
    // Cek apakah hanya berisi angka
    for (char c : norek) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

void inputTidakValid() {
    cout << "\n╔════════════════════════════════════════════╗\n";
    cout << "║         INPUT TIDAK VALID!                 ║\n";
    cout << "║      Silakan masukkan angka saja.          ║\n";
    cout << "╚════════════════════════════════════════════╝\n";
    cin.clear();
    cin.ignore(10000, '\n');
    cout << "Tekan Enter...";
    cin.get();
    return;
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
    cin.ignore();
    getline(cin, username);
    cout << "╠════════════════════════════════════════════╣\n";
    cout << "   Password: ";
    getline(cin, password);
    cout << "╚════════════════════════════════════════════╝\n";

    for (int i = 0; i < database["users"].size(); i++) {
        if (database["users"][i]["username"].asString() == username) {
            cout << "╔════════════════════════════════════════════╗\n";
            cout << "║          Username Sudah Terdaftar!         ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
            return;
        }
    }

    Json::Value newUser;
    newUser["username"] = username;
    newUser["password"] = password;
    newUser["role"] = "nasabah";
    newUser["poin"] = 0;

    database["users"].append(newUser);
    saveDatabase();
    cout << "╔════════════════════════════════════════════╗\n";
    cout << "║    Registrasi Berhasil, silahkan Login!    ║\n";
    cout << "╠════════════════════════════════════════════╣\n";
    cout << "\nRegistrasi berhasil. Silakan login.\n";
}

// Fungsi Logout
void logout() {
    currentUser = {};
    cout << "Anda telah logout.\n";
}

// Fungsi Untuk Menu Admin
void lihatBarangId(){
    cout << "\n╔═════════╦═══════════════════════════════════════════════╗\n";
    cout << "║    ID   ║                 Nama Barang                   ║\n";
    cout << "╠═════════╬═══════════════════════════════════════════════╣\n";
    
    for (const auto& barang : database["barang"]) {
        cout << "║ " << left << setw(7) << barang["id"].asString() 
             << " ║ " << left << setw(45) << barang["nama"].asString() << " ║\n";
    }
    cout << "╚═════════╩═══════════════════════════════════════════════╝\n\n";
}

void lihatDaftarBarang() {
    if (!database.isMember("barang") || database["barang"].empty()) {
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║      Belum ada data barang tersedia        ║\n";
        cout << "╚════════════════════════════════════════════╝\n";
        return;
    }

    cout << "\n╔═══════════════════════════════════════════════════╦═══════════╦═════════╗\n";
    cout << "║ Nama Barang                                       ║    Poin   ║  Stok   ║\n";
    cout << "╠═══════════════════════════════════════════════════╬═══════════╬═════════╣\n";

    for (const auto& barang : database["barang"]) {
        cout << "║ " << left << setw(49) << barang["nama"].asString() << " ║ "
             << right << setw(9) << barang["harga_poin"].asString() << " ║ "
             << right << setw(7) << barang["stok"].asString() << " ║\n";
    }
    cout << "╚═══════════════════════════════════════════════════╩═══════════╩═════════╝\n";
}

void tambahBarang() {
    string nama;
    int harga_poin, stok;

    cout << "\n╔════════════════════════════════════════════╗\n";
    cout << "║           TAMBAH BARANG BARU               ║\n";
    cout << "╠════════════════════════════════════════════╣\n";
    
    // Generate ID otomatis
    string newId = "BRG" + to_string(database["barang"].size() + 1);
    
    cin.ignore();
    cout << "   Nama Barang : ";
    getline(cin, nama);

    // Check for duplicate names (case insensitive)
    string namaLower = nama;
    transform(namaLower.begin(), namaLower.end(), namaLower.begin(), ::tolower);
    
    for (const auto& barang : database["barang"]) {
        string existingName = barang["nama"].asString();
        transform(existingName.begin(), existingName.end(), existingName.begin(), ::tolower);
        
        if (existingName == namaLower) {
            cout << "\n╔════════════════════════════════════════════╗\n";
            cout << "║      NAMA BARANG SUDAH ADA!               ║\n";
            cout << "║   Silakan gunakan nama barang lain.       ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
            cout << "\nTekan Enter untuk kembali...";
            cin.get();
            return;
        }
    }

    cout << "╠════════════════════════════════════════════╣\n";
    cout << "   Harga Poin  : ";
    cin >> harga_poin;
    cout << "╠════════════════════════════════════════════╣\n";
    cout << "   Stok        : ";
    cin >> stok;
    cout << "╚════════════════════════════════════════════╝\n";

    if (cin.fail() || harga_poin <= 0 || stok < 0) {
        inputTidakValid();
        return;
    }

    Json::Value barangBaru;
    barangBaru["id"] = newId;
    barangBaru["nama"] = nama;
    barangBaru["harga_poin"] = harga_poin;
    barangBaru["stok"] = stok;

    database["barang"].append(barangBaru);
    saveDatabase();
    
    cout << "\n╔════════════════════════════════════════════╗\n";
    cout << "║         BARANG BERHASIL DITAMBAHKAN!       ║\n";
    cout << "╚════════════════════════════════════════════╝\n";
    cout << "\nTekan Enter untuk kembali...";
    cin.ignore();
    cin.get();
}

void ubahBarang() {
    lihatBarangId();

    string id;
    cout << "\nMasukkan ID barang yang akan diubah: ";
    cin >> id;

    for (auto& barang : database["barang"]) {
        if (barang["id"].asString() == id) {
            cout << "\n╔════════════════════════════════════════════╗\n";
            cout << "║           UBAH DATA BARANG                 ║\n";
            cout << "╠════════════════════════════════════════════╣\n";
            cout << "   Nama Baru (sebelumnya: " << barang["nama"].asString() << "): ";
            cin.ignore();
            string nama;
            getline(cin, nama);
            if (!nama.empty()) barang["nama"] = nama;
            cout << "╠════════════════════════════════════════════╣\n";
            cout << "   Harga Poin Baru (sebelumnya: " << barang["harga_poin"].asInt() << "): ";
            int harga;
            cin >> harga;
            if (harga > 0) barang["harga_poin"] = harga;
            cout << "╠════════════════════════════════════════════╣\n";
            cout << "   Stok Baru (sebelumnya: " << barang["stok"].asInt() << "): ";
            int stok;
            cin >> stok;
            if (stok >= 0) barang["stok"] = stok;
            cout << "╚════════════════════════════════════════════╝\n";

            saveDatabase();
            cout << "Data barang berhasil diubah!\n";
            return;
        }
    }
    cout << "Barang dengan ID tersebut tidak ditemukan!\n";
}

void hapusBarang() {
    lihatBarangId();

    string id;
    cout << "\nMasukkan ID barang yang akan dihapus: ";
    cin >> id;

    Json::Value& barangArray = database["barang"];
    for (Json::ArrayIndex i = 0; i < barangArray.size(); i++) {
        if (barangArray[i]["id"].asString() == id) {
            cout << "Apakah anda yakin ingin menghapus barang '" 
                 << barangArray[i]["nama"].asString() << "'? (y/n): ";
            char konfirmasi;
            cin >> konfirmasi;
            
            if (konfirmasi == 'y' || konfirmasi == 'Y') {
                barangArray.removeIndex(i, &barangArray[barangArray.size() - 1]);
                saveDatabase();
                cout << "Barang berhasil dihapus!\n";
            } else {
                cout << "Penghapusan dibatalkan.\n";
            }
            return;
        }
    }
    cout << "Barang dengan ID tersebut tidak ditemukan!\n";
}

void manajemenBarang() {
    int pilihan;
    while (true) {
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║         MANAJEMEN DATA BARANG              ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║  1. Lihat Daftar Barang                    ║\n";
        cout << "║  2. Tambah Barang Baru                     ║\n";
        cout << "║  3. Ubah Data Barang                       ║\n";
        cout << "║  4. Hapus Barang                           ║\n";
        cout << "║  0. Kembali                                ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "   Pilih menu: ";
        cin >> pilihan;
        cout << "╚════════════════════════════════════════════╝\n";

        switch (pilihan) {
            case 1:
                lihatDaftarBarang();
                break;
            case 2:
                tambahBarang();
                break;
            case 3:
                ubahBarang();
                break;
            case 4:
                hapusBarang();
                break;
            case 0:
                return;
            default:
                pilihanTidakTersedia();
        }
    }
}

void manajemenTukarUang() {
    while (true) {
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║          MANAJEMEN TUKAR UANG              ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║  1. Lihat Daftar Penukaran Uang            ║\n";
        cout << "║  2. Tambah Saldo Uang Bank Sampah          ║\n";
        cout << "║  0. Kembali                                ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "   Pilih menu: ";
        int pilih;
        cin >> pilih;
        cout << "╚════════════════════════════════════════════╝\n";

        if (pilih == 1) {
            cout << "\nDaftar Penukaran Uang:\n";
            cout << "╔════╦═════════════════════╦════════════╦════════════╦═══════════════════════╦═════════════════════╗\n";
            cout << "║ No ║ Username            ║ Poin       ║ Nominal    ║ No Rekening           ║ Bank                 ║\n";
            cout << "╠════╬═════════════════════╬════════════╬════════════╬═══════════════════════╬═════════════════════╣\n";
            int no = 1;
            bool ada = false;
            for (const auto& trx : database["transactions"]) {
                if (trx.isMember("tipe") && trx["tipe"].asString() == "tukar_uang") {
                    cout << "║ " << setw(2) << right << no++ << " "
                         << "║ " << setw(20) << left << trx["username"].asString()
                         << "║ " << setw(10) << right << trx["poin_ditukar"].asInt()
                         << " ║ " << setw(10) << right << trx["nominal"].asInt()
                         << " ║ " << setw(22) << left << trx["no_rekening"].asString()
                         << "║ " << setw(20) << left << trx["bank"].asString() << "║\n";
                    ada = true;
                }
            }
            if (!ada) {
                cout << "║       Tidak ada data penukaran uang.                                                        ║\n";
            }
            cout << "╚════╩═════════════════════╩════════════╩════════════╩═══════════════════════╩═════════════════════╝\n";
        } else if (pilih == 2) {
            int tambah;
            cout << "Masukkan jumlah saldo uang yang ingin ditambahkan ke bank sampah: ";
            cin >> tambah;
            if (tambah > 0) {
                database["saldo_bank_sampah"] = database.get("saldo_bank_sampah", 0).asInt() + tambah;
                saveDatabase();
                cout << "Saldo berhasil ditambahkan.\n";
                cout << "Saldo bank sampah saat ini: Rp" << database["saldo_bank_sampah"].asInt() << endl;
            } else {
                cout << "Jumlah tidak valid.\n";
            }
        } else if (pilih == 0) {
            return;
        } else {
            cout << "Pilihan tidak tersedia!\n";
        }
    }
}


// ==== Fitur Manajemen Sampah ====

// Lihat daftar sampah
void lihatDaftarSampah() {
    if (!database.isMember("sampah") || database["sampah"].empty()) {
        cout << "\nBelum ada data sampah.\n";
        return;
    }
    cout << "\n╔════╦════════════════════════════╦════════════════════════════════════════════════════════╦═════════╗\n";
    cout << "║ No ║ Nama Sampah                ║ Deskripsi                                              ║ Poin    ║\n";
    cout << "╠════╬════════════════════════════╬════════════════════════════════════════════════════════╬═════════╣\n";
    int no = 1;
    for (const auto& s : database["sampah"]) {
        cout << "║ " << setw(2) << right << no++ << " "
             << "║ " << setw(27) << left << s["nama"].asString()
             << "║ " << setw(55) << left << s["deskripsi"].asString()
             << "║ " << setw(7) << right << s["poin_per_kg"].asInt() << " ║\n";
    }
    cout << "╚════╩════════════════════════════╩════════════════════════════════════════════════════════╩═════════╝\n";
}

// Tambah data sampah
void updateWastePrice() {
    while (true) {
        system("cls");
        lihatDaftarSampah();
        
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║         UPDATE HARGA POIN SAMPAH           ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║  Masukkan nama sampah (0 untuk batal):     ║\n";
        cout << "╚════════════════════════════════════════════╝\n";
        cout << "→ ";
        
        string namaSampah;
        // Hapus cin.ignore() di sini karena bisa menyebabkan skip input
        getline(cin >> ws, namaSampah);  // Gunakan ws untuk mengabaikan whitespace

        if (namaSampah == "0") {
            return;  // Kembali ke menu sebelumnya
        }

        string inputLower = namaSampah;
        transform(inputLower.begin(), inputLower.end(), inputLower.begin(), ::tolower);

        bool found = false;
        for (auto& sampah : database["sampah"]) {
            string dbNama = sampah["nama"].asString();
            string dbNamaLower = dbNama;
            transform(dbNamaLower.begin(), dbNamaLower.end(), dbNamaLower.begin(), ::tolower);

            if (dbNamaLower == inputLower) {
                found = true;
                cout << "\nHarga poin saat ini: " << sampah["poin_per_kg"].asInt() << " poin/kg\n";
                cout << "Masukkan harga poin baru: ";
                int hargaBaru;
                
                if (!(cin >> hargaBaru)) {
                    inputTidakValid();
                    break;
                }

                if (hargaBaru <= 0) {
                    cout << "\n╔════════════════════════════════════════════╗\n";
                    cout << "║       HARGA TIDAK BOLEH NOL/NEGATIF!       ║\n";
                    cout << "╚════════════════════════════════════════════╝\n";
                    cout << "\nTekan Enter untuk kembali...";
                    cin.ignore();
                    cin.get();
                    break;
                }

                sampah["poin_per_kg"] = hargaBaru;
                saveDatabase();

                cout << "\n╔════════════════════════════════════════════╗\n";
                cout << "║      HARGA POIN BERHASIL DIUPDATE!         ║\n";
                cout << "╚════════════════════════════════════════════╝\n";
                cout << "\nTekan Enter untuk kembali...";
                cin.ignore();
                cin.get();
                break;
            }
        }

        if (!found) {
            cout << "\n╔════════════════════════════════════════════╗\n";
            cout << "║         SAMPAH TIDAK DITEMUKAN!            ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
            cout << "\nTekan Enter untuk kembali...";
            cin.get();
        }
    }
}
void manajemenDataSampah() {
    while (true) {
        system("cls");
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║         MANAJEMEN DATA SAMPAH              ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║  1. Lihat Data Sampah                      ║\n";
        cout << "║  2. Update Harga Poin Sampah               ║\n";
        cout << "║  0. Kembali                                ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "   Pilih: ";
        
        int choice;
        cin >> choice;
        cout << "╚════════════════════════════════════════════╝\n";

        if (cin.fail()) {
            inputTidakValid();
            continue;
        }

        switch (choice) {
            case 0:
                return;
            case 1:
                lihatDaftarSampah();
                cout << "\nTekan Enter untuk kembali...";
                cin.ignore();
                cin.get();
                break;
            case 2:
                updateWastePrice();
                break;
            default:
                cout << "\n╔════════════════════════════════════════════╗\n";
                cout << "║         PILIHAN TIDAK TERSEDIA!            ║\n";
                cout << "╚════════════════════════════════════════════╝\n";
                cout << "\nTekan Enter untuk kembali...";
                cin.ignore();
                cin.get();
                break;
        }
    }
}


// === Fitur Manajemen Petugas ===

// Lihat Daftar Petugas
void lihatDaftarPetugas() {
    cout << "\n╔════╦═══════════════════════════════════════╗\n";
    cout << "║ No ║            DAFTAR PETUGAS             ║\n";
    cout << "╠════╬═══════════════════════════════════════╣\n";
    int no = 1;
    for (auto& p : database["users"]) {
        if (p["role"].asString() == "petugas") {
            cout << "║ " << setw(2) << right << no++ << " " 
                 << "║  " << setw(35) << left << p["username"].asString() << "  ║\n";
        }
    }
    cout << "╚════╩═══════════════════════════════════════╝\n";
}

// Tambah Data Petugas
void tambahPetugas() {
    string username, password, role;

    cout << "\n╔════════════════════════════════════════════╗\n";
    cout << "║           TAMBAH PETUGAS BARU              ║\n";
    cout << "╠════════════════════════════════════════════╣\n";
    
    
    cin.ignore();
    cout << "   Username : ";
    getline(cin, username);
    
    for (const auto& users : database["users"]) {
        string existingName = users["username"].asString();
        
        if (existingName == username) {
            cout << "\n╔════════════════════════════════════════════╗\n";
            cout << "║          USERNAME SUDAH ADA!               ║\n";
            cout << "║    Silakan gunakan username lain.          ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
            cout << "\nTekan Enter untuk kembali...";
            cin.get();
            return;
        }
    }

    cout << "╠════════════════════════════════════════════╣\n";
    cout << "   Password  : ";
    getline(cin, password);
    cout << "╚════════════════════════════════════════════╝\n";

    Json::Value usersBaru;
    usersBaru["username"] = username;
    usersBaru["password"] = password;
    usersBaru["role"] = "petugas";

    database["users"].append(usersBaru);
    saveDatabase();
    
    cout << "\n╔════════════════════════════════════════════╗\n";
    cout << "║         PETUGAS BERHASIL DITAMBAHKAN!      ║\n";
    cout << "╚════════════════════════════════════════════╝\n";
    cout << "\nTekan Enter untuk kembali...";
    cin.ignore();
    cin.get();
}

// Ubah Data Petugas
void ubahPetugas() {
    lihatDaftarPetugas();

    string username;
    cout << "\nMasukkan username petugas yang ingin diubah: ";
    cin.ignore();
    getline(cin, username);

    for (auto& users : database["users"]) {
        if (users["username"].asString() == username && users["role"].asString() == "petugas") {
            cout << "\n╔════════════════════════════════════════════╗\n";
            cout << "║              UBAH DATA PETUGAS             ║\n";
            cout << "╠════════════════════════════════════════════╣\n";
            cout << "   Username Baru (sebelumnya: " << users["username"].asString() << "): ";
            cin.ignore();
            string nama;
            getline(cin, nama);
            if (!nama.empty()) users["username"] = nama;
            cout << "╠════════════════════════════════════════════╣\n";
            cout << "   Password Baru (sebelumnya: " << users["password"].asString() << "): ";
            string password;
            getline(cin, password);
            if (!password.empty()) users["password"] = password;
            cout << "╚════════════════════════════════════════════╝\n";

            saveDatabase();
            cout << "Data petugas berhasil diubah!\n";
            return;
        }
    }
    cout << "Petugas tidak ditemukan!\n";
}

// Hapus Data Petugas
void hapusPetugas() {
    lihatDaftarPetugas();

    string username;
    cout << "\nMasukkan username petugas yang ingin dihapus: ";
    cin.ignore();
    getline(cin, username);

    Json::Value& petugasArray = database["users"];
    for (Json::ArrayIndex i = 0; i < petugasArray.size(); i++) {
        if (petugasArray[i]["username"].asString() == username && petugasArray[i]["role"].asString() == "petugas") {
            cout << "Apakah anda yakin ingin menghapus petugas '" 
                 << petugasArray[i]["username"].asString() << "'? (y/n): ";
            char konfirmasi;
            cin >> konfirmasi;
            
            if (konfirmasi == 'y' || konfirmasi == 'Y') {
                petugasArray.removeIndex(i, &petugasArray[petugasArray.size() - 1]);
                saveDatabase();
                cout << "Petugas berhasil dihapus!\n";
            } else {
                cout << "Penghapusan dibatalkan.\n";
            }
            return;
        }
    }
    cout << "Username tidak ditemukan!\n";
}

// Menu Manajemen Petugas
void manajemenPetugas() {
    int pilih;
    while (true) {
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║           MANAJEMEN DATA PETUGAS           ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║  1. Lihat Daftar Petugas                   ║\n";
        cout << "║  2. Tambah Data Petugas                    ║\n";
        cout << "║  3. Ubah Data Petugas                      ║\n";
        cout << "║  4. Hapus Data Petugas                     ║\n";
        cout << "║  0. Kembali                                ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "   Pilih menu: ";
        cin >> pilih;
        cout << "╚════════════════════════════════════════════╝\n";
        switch (pilih) {
            case 1: 
                lihatDaftarPetugas(); 
                break;
            case 2: 
                tambahPetugas(); 
                break;
            case 3: 
                ubahPetugas(); 
                break;
            case 4: 
                hapusPetugas(); 
                break;
            case 0: 
                return;
            default: 
                cout << "Pilihan tidak tersedia!\n";
        }
    }
}

void laporanPenerimaanSampah() {
    map<string, pair<double, int>> totalSampah; // jenis_sampah -> {total_kg, total_poin}
    
    cout << "\n╔══════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                              LAPORAN PENERIMAAN SAMPAH                           ║\n";
    cout << "╠═══════════════════════╦══════════════════╦═══════════════════╦═══════════════════╣\n";
    cout << "║     Jenis Sampah      ║    Total (KG)    ║    Total Poin     ║     Nilai (Rp)    ║\n";
    cout << "╠═══════════════════════╬══════════════════╬═══════════════════╬═══════════════════╣\n";

    // Hitung total per jenis sampah
    for (const auto& trx : database["transactions"]) {
        if (trx.isMember("jenis_sampah")) {
            string jenis = trx["jenis_sampah"].asString();
            double berat = trx["berat_kg"].asDouble();
            int poin = trx["poin_diterima"].asInt();
            totalSampah[jenis].first += berat;
            totalSampah[jenis].second += poin;
        }
    }

    // Tampilkan hasil
    double totalBeratAll = 0;
    int totalPoinAll = 0;
    for (const auto& item : totalSampah) {
        cout << "║ " << left << setw(22) << item.first 
             << "║ " << right << setw(17) << fixed << setprecision(2) << item.second.first 
             << "║ " << right << setw(18) << item.second.second 
             << "║ " << right << setw(18) << item.second.second * 100 << "║\n";
        totalBeratAll += item.second.first;
        totalPoinAll += item.second.second;
    }

    cout << "╠═══════════════════════╬══════════════════╬═══════════════════╬═══════════════════╣\n";
    cout << "║ TOTAL                 ║ " << right << setw(17) << totalBeratAll 
         << "║ " << right << setw(18) << totalPoinAll 
         << "║ " << right << setw(18) << totalPoinAll * 100 << "║\n";
    cout << "╚═══════════════════════╩══════════════════╩═══════════════════╩═══════════════════╝\n";
}

void laporanPenukaranUang() {
    map<string, pair<int, int>> totalTukarUang; // bank -> {total_poin, total_nominal}
    
    cout << "\n╔═════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                   LAPORAN PENUKARAN POIN KE UANG                    ║\n";
    cout << "╠═══════════════════════════════╦══════════════════╦══════════════════╣\n";
    cout << "║ Bank Tujuan                   ║    Total Poin    ║    Total (Rp)    ║\n";
    cout << "╠═══════════════════════════════╬══════════════════╬══════════════════╣\n";

    for (const auto& trx : database["transactions"]) {
        if (trx.isMember("tipe") && trx["tipe"].asString() == "tukar_uang") {
            string bank = trx["bank"].asString();
            int poin = trx["poin_ditukar"].asInt();
            int nominal = trx["nominal"].asInt();
            totalTukarUang[bank].first += poin;
            totalTukarUang[bank].second += nominal;
        }
    }

    int totalPoinTukar = 0, totalNominal = 0;
    for (const auto& item : totalTukarUang) {
        cout << "║ " << left << setw(29) << item.first 
             << " ║ " << right << setw(16) << item.second.first 
             << " ║ " << right << setw(16) << item.second.second << " ║\n";
        totalPoinTukar += item.second.first;
        totalNominal += item.second.second;
    }
    
    cout << "╠═══════════════════════════════╬══════════════════╬══════════════════╣\n";
    cout << "║ TOTAL                         ║ " << right << setw(16) << totalPoinTukar 
         << " ║ " << right << setw(16) << totalNominal << " ║\n";
    cout << "╚═══════════════════════════════╩══════════════════╩══════════════════╝\n";
    cout << "\n╔═══════════════════════════════════════════════════════╗\n";
    cout << "║  Total Transaksi Penukaran Uang: " << left << setw(21) << totalTukarUang.size() << "║\n";
    cout << "╚═══════════════════════════════════════════════════════╝\n";
}

void laporanPenukaranBarang() {
    map<string, pair<int, int>> totalTukarBarang; // nama_barang -> {jumlah, total_poin}
    
    cout << "\n╔══════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                      LAPORAN PENUKARAN BARANG                        ║\n";
    cout << "╠════════════════════════════════════╦════════════╦════════════════════╣\n";
    cout << "║ Nama Barang                        ║   Jumlah   ║    Total Poin      ║\n";
    cout << "╠════════════════════════════════════╬════════════╬════════════════════╣\n";

    for (const auto& trx : database["transactions"]) {
        if (trx.isMember("tipe") && trx["tipe"].asString() == "tukar_barang") {
            string namaBarang = trx["nama_barang"].asString();
            int jumlah = trx["jumlah"].asInt();
            int poin = trx["harga_poin"].asInt() * jumlah;
            totalTukarBarang[namaBarang].first += jumlah;
            totalTukarBarang[namaBarang].second += poin;
        }
    }

    int totalJumlahBarang = 0, totalPoinBarang = 0;
    for (const auto& item : totalTukarBarang) {
        cout << "║ " << left << setw(34) << item.first 
             << " ║ " << right << setw(10) << item.second.first
             << " ║ " << right << setw(18) << item.second.second << " ║\n";
        totalJumlahBarang += item.second.first;
        totalPoinBarang += item.second.second;
    }
    
    cout << "╠════════════════════════════════════╬════════════╬════════════════════╣\n";
    cout << "║ TOTAL                              ║ " 
         << right << setw(10) << totalJumlahBarang
         << " ║ " << right << setw(18) << totalPoinBarang << " ║\n";
    cout << "╚════════════════════════════════════╩════════════╩════════════════════╝\n";

    cout << "\n╔════════════════════════════════════════════════════════╗\n";
    cout << "║               TOTAL NILAI PENUKARAN                    ║\n";
    cout << "╠════════════════════════════════╦═══════════════════════╣\n";
    cout << "║ Total Poin                     ║ " << right << setw(21) << totalPoinBarang << " ║\n";
    cout << "║ Total Nilai (Rp)               ║ " << right << setw(21) << totalPoinBarang * 100 << " ║\n";
    cout << "╚════════════════════════════════╩═══════════════════════╝\n";
}

// Laporan Transaksi
void laporanTransaksi(){
    int pilihan;
    while (true) {
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║            LAPORAN TRANSAKSI               ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║  1. Laporan Penerimaan Sampah              ║\n";
        cout << "║  2. Laporan Penukaran Poin ke Uang         ║\n";
        cout << "║  3. Laporan Penukaran Barang               ║\n";
        cout << "║  0. Kembali                                ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "   Pilih menu: ";
        cin >> pilihan;
        cout << "╚════════════════════════════════════════════╝\n";

        switch (pilihan) {
            case 1:
                laporanPenerimaanSampah();
                break;
            case 2:
                laporanPenukaranUang();
                break;
            case 3:
                laporanPenukaranBarang();
                break;
            case 0:
                return;
            default:
                cout << "Pilihan tidak tersedia!\n";
        }
    }
}

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
        cout << "║  4. Manajemen Petugas                  ║\n";
        cout << "║  5. Laporan Transaksi                  ║\n";
        cout << "║  0. Keluar                             ║\n";
        cout << "╠════════════════════════════════════════╣\n";
        cout << "   Pilih menu: ";
        cin >> choice;
        cout << "╚════════════════════════════════════════╝\n";

        switch (choice) {
            case 1: 
                manajemenBarang();
                break;
            case 2: 
                manajemenTukarUang(); 
                break;
            case 3:
                manajemenDataSampah();
                break;
            case 4:
                manajemenPetugas();
                break;
            case 5:
                laporanTransaksi();
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
        cout << "\n[KESALAHAN] Data sampah tidak tersedia." << endl;
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

    cout << "\nklik enter untuk kembali...";
    cin.ignore();
    cin.get();
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
    cout << "\nTekan Enter untuk kembali...";
    cin.ignore();
}

void riwayatTransaksi() {
    cout << "\n╔════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                                    RIWAYAT TRANSAKSI                                       ║\n";
    cout << "╠════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    cout << "║ Poin Anda saat ini: " << setw(71) << left << currentUser.poin << "║\n";
    cout << "╠════════════════════════════════════════════════════════════════════════════════════════════╣\n";

    bool ada = false;
    for (const auto& trx : database["transactions"]) {
        if (trx["username"].asString() == currentUser.username) {
            cout << "║ ID: " << setw(85) << left << trx["id_transaksi"].asString() << "  ║\n";
            
            if (trx.isMember("tipe")) {
                if (trx["tipe"].asString() == "tukar_uang") {
                    cout << "║ Tipe Transaksi : Penukaran Uang" << string(60, ' ') << "║\n";
                    cout << "║ Poin Ditukar   : " << setw(71) << left << trx["poin_ditukar"].asInt() << "   ║\n";
                    cout << "║ Nominal        : Rp" << setw(69) << left << trx["nominal"].asInt() << "   ║\n";
                    cout << "║ No Rekening    : " << setw(71) << left << trx["no_rekening"].asString() << "   ║\n";
                    cout << "║ Bank           : " << setw(71) << left << trx["bank"].asString() << "   ║\n";
                } 
                else if (trx["tipe"].asString() == "tukar_barang") {
                    cout << "║ Tipe Transaksi : Penukaran Barang" << string(58, ' ') << "║\n";
                    cout << "║ Nama Barang    : " << setw(71) << left << trx["nama_barang"].asString() << "   ║\n";
                    cout << "║ Harga Poin     : " << setw(71) << left << trx["harga_poin"].asInt() << "   ║\n";
                    if (trx.isMember("jumlah")) {
                        cout << "║ Jumlah         : " << setw(71) << left << trx["jumlah"].asInt() << "   ║\n";
                    }
                }
            } else {
                cout << "║ Tipe Transaksi : Setor Sampah" << string(62, ' ') << "║\n";
                cout << "║ Jenis Sampah   : " << setw(71) << left << trx["jenis_sampah"].asString() << "   ║\n";
                cout << "║ Berat          : " << trx["berat_kg"].asDouble() << " kg" << string(65, ' ') << "    ║\n";
                cout << "║ Poin Diterima  : " << setw(71) << left << trx["poin_diterima"].asInt() << "   ║\n";
            }

            if (trx.isMember("tanggal")) {
                cout << "║ Tanggal        : " << setw(71) << left << trx["tanggal"].asString() << "   ║\n";
            }
            cout << "╠════════════════════════════════════════════════════════════════════════════════════════════╣\n";
            ada = true;
        }
    }

    if (!ada) {
        cout << "║" << string(92, ' ') << "║\n";
        cout << "║" << setw(49) << right << "Belum ada transaksi." << string(43, ' ') << "║\n";
        cout << "║" << string(92, ' ') << "║\n";
    }
    
    cout << "╚════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    cout << "\nTekan Enter untuk kembali...";
    cin.ignore();
    cin.get();
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
    cout << "\nTekan Enter untuk kembali...";
    cin.ignore();
    cin.get();
}

void menukarPoin() {
while (true) {    
    cout << "\n╔════════════════════════════════════════════╗\n";
    cout << "║            MENU TUKAR POIN                 ║\n";
    cout << "╠════════════════════════════════════════════╣\n";
    cout << "║  Poin Anda saat ini: " << setw(20) << left << currentUser.poin << "  ║\n";
    cout << "╠════════════════════════════════════════════╣\n";
    cout << "║  1. Tukar dengan Uang                      ║\n";
    cout << "║  2. Tukar dengan Barang                    ║\n";
    cout << "║  0. Kembali                                ║\n";
    cout << "╠════════════════════════════════════════════╣\n";
    cout << "   Pilih: ";
    int pilihTukar;
    cin >> pilihTukar;
    cout << "╚════════════════════════════════════════════╝\n";

    if (cin.fail()) {
        inputTidakValid();
        continue;
        }

    if (pilihTukar < 0 || pilihTukar > 2) {
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║         PILIHAN TIDAK TERSEDIA!            ║\n";
        cout << "╚════════════════════════════════════════════╝\n";
        cout << "\nTekan Enter untuk kembali...";
        cin.ignore();
        cin.get();
        continue;
    }  

    if (pilihTukar == 1) {
    while (true) {
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║           PENUKARAN POIN KE UANG           ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║  Poin Anda saat ini: " << setw(20) << left << currentUser.poin << "  ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << " Jumlah poin yang ingin ditukar(0 untuk batal): ";
        int jumlahTukar;
        cin >> jumlahTukar;

        if (cin.fail()) {
            inputTidakValid();
            continue;
            }

        if (jumlahTukar > 0 && jumlahTukar <= currentUser.poin) {
            string norek, bank;
            cout << " Nomor rekening tujuan: ";
            cin.ignore();
            getline(cin, norek);
            
            if (!isValidRekening(norek)) {
            cout << "\n╔════════════════════════════════════════════╗\n";
            cout << "║  NOMOR REKENING TIDAK VALID!               ║\n";
            cout << "║  • Panjang harus 10-16 digit               ║\n";
            cout << "║  • Hanya boleh berisi angka                ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
            cin.clear();
            cout << "\nTekan Enter untuk kembali...";
            cin.get();
            continue;
            }
                
            cout << "\n╔════════════════════════════════════════════╗\n";
            cout << "║             PILIHAN BANK                   ║\n";
            cout << "╠════════════════════════════════════════════╣\n";
            cout << "║  1. BRI                                    ║\n";
            cout << "║  2. BNI                                    ║\n";
            cout << "║  3. Mandiri                                ║\n";
            cout << "║  4. BCA                                    ║\n";
            cout << "║  5. Bank Kaltimtara                        ║\n";
            cout << "║  6. Bank lainnya                           ║\n";
            cout << "╠════════════════════════════════════════════╣\n";
            cout << "   Pilih bank (1-6): ";
            int pilihBank;
            cin >> pilihBank;
            cout << "╚════════════════════════════════════════════╝\n";

            if (cin.fail()) {
                inputTidakValid();
                continue;
            }

            // Tentukan nama bank berdasarkan pilihan
            switch(pilihBank) {
                case 1: bank = "BRI"; break;
                case 2: bank = "BNI"; break;
                case 3: bank = "Mandiri"; break;
                case 4: bank = "BCA"; break;
                case 5: bank = "Bank Kaltimtara"; break;
                case 6: 
                    cout << "Masukkan nama bank: ";
                    cin.ignore();
                    getline(cin, bank);
                    break;
                default:
                    cout << "\n╔════════════════════════════════════════════╗\n";
                    cout << "║         PILIHAN BANK TIDAK TERSEDIA!       ║\n";
                    cout << "╚════════════════════════════════════════════╝\n";
                    cout << "\nTekan Enter untuk kembali...";
                    cin.ignore();
                    cin.get();
                    continue;
            }

            int uang = jumlahTukar * 10;
            int saldoBank = database.get("saldo_bank_sampah", 0).asInt();

            if (saldoBank < uang) {
                cout << "\n╔════════════════════════════════════════════╗\n";
                cout << "║MAAF, Layanan pertukaran uang saat ini tidak║\n";
                cout << "║tersedia untuk nominal pertukaran anda      ║\n";
                cout << "║Max penukaran poin saat ini : " << setw(14) << left << saldoBank/10 << "║\n";
                cout << "╚════════════════════════════════════════════╝\n";
                cout << "\nTekan Enter untuk kembali...";
                cin.ignore();
                cin.get();
                continue;
            }

            currentUser.poin -= jumlahTukar;
            database["saldo_bank_sampah"] = saldoBank - uang;  // Kurangi saldo bank

            // Update database user
            for (auto& user : database["users"]) {
                if (user["username"].asString() == currentUser.username) {
                    user["poin"] = currentUser.poin;
                    break;
                }
            }
            // Tambahkan transaksi penukaran uang ke database
            Json::Value transaksiBaru;
            transaksiBaru["id_transaksi"] = "TRXU" + to_string(database["transactions"].size() + 1);
            transaksiBaru["username"] = currentUser.username;
            transaksiBaru["tipe"] = "tukar_uang";
            transaksiBaru["poin_ditukar"] = jumlahTukar;
            transaksiBaru["nominal"] = uang;
            transaksiBaru["no_rekening"] = norek;
            transaksiBaru["bank"] = bank;
            database["transactions"].append(transaksiBaru);

            saveDatabase();
            cout << "\n╔════════════════════════════════════════════╗\n";
            cout << "║          PERTUKARAN BERHASIL!              ║\n";
            cout << "╠════════════════════════════════════════════╣\n";
            cout << "║  Detail Transaksi:                         ║\n";
            cout << "║  • Poin Ditukar   : " << setw(20) << left << jumlahTukar << "   ║\n";
            cout << "║  • Nominal Uang   : Rp " << setw(18) << left << uang << "  ║\n";
            cout << "║  • No. Rekening   : " << setw(20) << left << norek << "   ║\n";
            cout << "║  • Bank           : " << setw(20) << left << bank << "   ║\n";
            cout << "╠════════════════════════════════════════════╣\n";
            cout << "║  Sisa Poin Anda   : " << setw(20) << left << currentUser.poin << "   ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
            cin.ignore();
            cout << "\nTekan Enter untuk kembali ke menu...";
            cin.get();
            
        } else if(jumlahTukar == 0) {
            break;
        } else {
            cout << "\n╔════════════════════════════════════════════╗\n";
            cout << "║    Jumlah Poin Tidak Valid/Tidak Cukup!!   ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
            cout << "\nTekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
            continue;
        }
    }
    } else if (pilihTukar == 2) {
        while (true) {
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║         PENUKARAN POIN KE BARANG           ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║  Poin Anda saat ini: " << setw(20) << left << currentUser.poin << "  ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║           URUTKAN BERDASARKAN              ║\n";
        cout << "║  1. Nama (A-Z)                             ║\n";
        cout << "║  2. Harga (Termurah)                       ║\n";
        cout << "║  3. Stok (Terbanyak)                       ║\n";
        cout << "║  0. Kembali                                ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "   Pilih: ";
        int urut;
        cin >> urut;
        cout << "╚════════════════════════════════════════════╝\n";
        
        if (cin.fail()) {
            inputTidakValid();
            continue;
        }
        
        if (urut == 0) {
            break; 
        } 
        if (urut < 0) {
            cout << "\n╔════════════════════════════════════════════╗\n";
            cout << "║         PILIHAN TIDAK BOLEH NEGATIF!       ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
            cout << "\nTekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
            continue;
        }
        
        while (true) {
        if (urut == 1) {
            system("cls");
            cout << "\n╔═════════════════════════════════════════════════════════╗\n";
            cout << "║           MENGURUTKAN BERDASARKAN NAMA (A-Z)            ║\n";
            cout << "╚═════════════════════════════════════════════════════════╝\n";
        } else if (urut == 2) {
            system("cls");
            cout << "\n╔═════════════════════════════════════════════════════════╗\n";
            cout << "║              MENGURUTKAN BERDASARKAN HARGA              ║\n";
            cout << "╚═════════════════════════════════════════════════════════╝\n";
        } else if (urut == 3) {
            system("cls");
            cout << "\n╔═════════════════════════════════════════════════════════╗\n";
            cout << "║              MENGURUTKAN BERDASARKAN STOK               ║\n";
            cout << "╚═════════════════════════════════════════════════════════╝\n";
        } else {
            system("cls");
            cout << "\n╔═════════════════════════════════════════════════════════╗\n";
            cout << "║              Menampilkan Tanpa Pengurutan               ║\n";
            cout << "╚═════════════════════════════════════════════════════════╝\n";
            
        }

        // Buat array index urutannyaS
        int n = database["barang"].size();
        int urutan[n];
        for (int i = 0; i < n; ++i) urutan[i] = i;
        
        // Bubble Sort untuk urutan barang
        for (int i = 0; i < n-1; ++i) {
            for (int j = 0; j < n-i-1; ++j) {
                bool tukar = false;
                if (urut == 1) {
                    if (database["barang"][urutan[j]]["nama"].asString() > database["barang"][urutan[j+1]]["nama"].asString())
                        tukar = true;
                    } else if (urut == 2) {
                        if (database["barang"][urutan[j]]["harga_poin"].asInt() > database["barang"][urutan[j+1]]["harga_poin"].asInt())
                        tukar = true;
                } else if (urut == 3) {
                    if (database["barang"][urutan[j]]["stok"].asInt() < database["barang"][urutan[j+1]]["stok"].asInt())
                    tukar = true;
                }
                
                if (tukar) {
                    int temp = urutan[j];
                    urutan[j] = urutan[j+1];
                    urutan[j+1] = temp;
                }
            }
        }
        
        cout << "\n╔════╦═══════════════════════════════╦════════════╦═══════╗\n";
        cout << "║ No ║ Nama Barang                   ║ HargaPoin  ║ Stok  ║\n";
        cout << "╠════╬═══════════════════════════════╬════════════╬═══════╣\n";
        for (int i = 0; i < n; ++i) {
            int idx = urutan[i];
            cout << "║ " << setw(2) << right << (i+1) << " "
                 << "║ " << setw(30) << left << database["barang"][idx]["nama"].asString()
                 << "║ " << setw(10) << right << database["barang"][idx]["harga_poin"].asInt()
                 << " ║ " << setw(5) << right << database["barang"][idx]["stok"].asInt() << " ║\n";
        }
        cout << "╚════╩═══════════════════════════════╩════════════╩═══════╝\n";

        cout << "Poin Anda saat ini: " << currentUser.poin << endl;
        cout << "Pilih nomor barang yang ingin ditukar (0 untuk batal): ";
        int pilihBarang;
        cin >> pilihBarang;
        
        if (cin.fail()) {
            inputTidakValid();
            continue;
        }

        if (pilihBarang > 0 && pilihBarang <= n) {
            int idx = urutan[pilihBarang-1];
            int harga = database["barang"][idx]["harga_poin"].asInt();
            int stok = database["barang"][idx]["stok"].asInt();
            if (stok <= 0) {
                cout << "Stok barang habis. Silakan pilih barang lain.\n";
            } else {
                int jumlahTukar;
                cout << "Masukkan jumlah barang yang ingin ditukar: ";
                cin >> jumlahTukar;

                if (cin.fail()) {
                    inputTidakValid();
                    continue;
                }

                if (jumlahTukar <= 0) {
                    cout << "\n╔════════════════════════════════════════════╗\n";
                    cout << "║       JUMLAH TIDAK BOLEH KURANG DARI 1!    ║\n";
                    cout << "╚════════════════════════════════════════════╝\n";
                    cout << "\nTekan Enter untuk kembali...";
                    cin.ignore();
                    cin.get();
                    continue;
                } else if (jumlahTukar > stok) {
                    cout << "\n╔════════════════════════════════════════════╗\n";
                    cout << "║       JUMLAH TIDAK BOLEH MELEBIHI STOK!    ║\n";
                    cout << "╚════════════════════════════════════════════╝\n";
                    cout << "\nTekan Enter untuk kembali...";
                    cin.ignore();
                    cin.get();
                    continue;
                } else if (currentUser.poin < harga * jumlahTukar) {
                    cout << "\n╔════════════════════════════════════════════╗\n";
                    cout << "║       POIN ANDA TIDAK CUKUP UNTUK TUKAR!   ║\n";
                    cout << "║       Harga Poin: " << harga << " x " << jumlahTukar << " = " << harga * jumlahTukar << " Poin      ║\n"; 
                    cout << "╚════════════════════════════════════════════╝\n";
                    cout << "\nTekan Enter untuk kembali...";
                    cin.ignore();
                    cin.get();
                    continue;
                } else {
                    currentUser.poin -= harga * jumlahTukar;
                    database["barang"][idx]["stok"] = stok - jumlahTukar;
                    // Update database user
                    for (auto& user : database["users"]) {
                        if (user["username"].asString() == currentUser.username) {
                            user["poin"] = currentUser.poin;
                            break;
                        }
                    }
                    // Tambahkan transaksi penukaran barang ke database
                    Json::Value transaksiBaru;
                    transaksiBaru["id_transaksi"] = "TRXB" + to_string(database["transactions"].size() + 1);
                    transaksiBaru["username"] = currentUser.username;
                    transaksiBaru["tipe"] = "tukar_barang";
                    transaksiBaru["nama_barang"] = database["barang"][idx]["nama"].asString();
                    transaksiBaru["harga_poin"] = harga;
                    transaksiBaru["jumlah"] = jumlahTukar;
                    // transaksiBaru["tanggal"] = "2025-05-11"; // opsional
                    database["transactions"].append(transaksiBaru);

                    saveDatabase();
                    cout << "\n╔═══════════════════════════════════════════════════════╗\n";
                    cout << "║                  PERTUKARAN BERHASIL!                 ║\n";
                    cout << "╠═══════════════════════════════════════════════════════╣\n";
                    cout << "║  Detail Transaksi:                                    ║\n";
                    cout << "║  • Nama Barang    : " << setw(31) << left << database["barang"][idx]["nama"].asString() << "   ║\n";
                    cout << "║  • Harga Poin     : " << setw(31) << left << harga << "   ║\n";
                    cout << "║  • Jumlah         : " << setw(31) << left << jumlahTukar << "   ║\n";
                    cout << "║  • Total Poin     : " << setw(31) << left << (harga * jumlahTukar) << "   ║\n";
                    cout << "╠═══════════════════════════════════════════════════════╣\n";
                    cout << "║  Sisa Poin Anda   : " << setw(31) << left << currentUser.poin << "   ║\n";
                    cout << "║  Sisa Stok Barang : " << setw(31) << left << (stok - jumlahTukar) << "   ║\n";
                    cout << "╚═══════════════════════════════════════════════════════╝\n";
                    cout << "\nTekan Enter...";
                    cin.ignore();
                    cin.get();
                }
            }
        } else if (pilihBarang == 0) {
            cout << "Batal menukar barang.\n";
            break;
        } else {
            cout << "\n╔════════════════════════════════════════════╗\n";
            cout << "║         PILIHAN TIDAK TERSEDIA!            ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
            cout << "\nTekan Enter...";
            cin.ignore();
            cin.get();
            continue;
        }
    }
}
    } else if (pilihTukar == 0) {
        cout << "Kembali ke menu nasabah.\n";
        break;
    } else {
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║         PILIHAN TIDAK TERSEDIA!            ║\n";
        cout << "╚════════════════════════════════════════════╝\n";
        cout << "\nTekan Enter...";
        cin.ignore();
        cin.get();
        continue;
    }
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

        if (cin.fail()) {
            inputTidakValid();
            continue;
            }

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
    string username;
    double beratKg;
    int jenisSampah;

    cout << "Masukkan username nasabah: ";
    cin.ignore();
    getline(cin, username);

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

    const Json::Value& daftarSampah = database["sampah"];
    const int noColWidth = 5;        
    const int jenisColWidth = 20;   
    const int descColWidth = 75;     
    const int poinColWidth = 10;
    
    // Buat array index urutannyaS
        int n = database["sampah"].size();
        int urutan[n];
        for (int i = 0; i < n; ++i) urutan[i] = i;



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

    for (int i = 0; i < n; ++i) {
        string nama = database["sampah"][urutan[i]]["nama"].asString();
        string deskripsi = database["sampah"][urutan[i]]["deskripsi"].asString();
        int poin = database["sampah"][urutan[i]]["poin_per_kg"].asInt();

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
        cout << "║ " << right << setw(3) << i+1 << "  "  // Format nomor dengan lebar 3 + 2 spasi
             << "║ " << left << setw(jenisColWidth) << namaTampil 
             << "║ " << left << setw(descColWidth) << deskripsiTampil
             << "║ " << right << setw(poinColWidth) << poin << " ║\n";

    }

    // Baris bawah tabel
    cout << "╚══════╩═════════════════════╩════════════════════════════════════════════════════════════════════════════╩════════════╝\n";

    cout << "Masukkan nomor jenis sampah: ";
    cin.ignore();
    cin >> jenisSampah;

    if (cin.fail()) {
        inputTidakValid();
        return;
    }

    if (jenisSampah < 1 || jenisSampah > daftarSampah.size()) {
        cout << "Nomor jenis sampah tidak valid.\n";
        return;
    }

    cout << "Masukkan berat sampah (kg): ";
    cin >> beratKg;

    if (cin.fail()) {
        inputTidakValid();
        return;
    }

    if (beratKg <= 0) {
        cout << "Berat sampah harus lebih besar dari 0 kg.\n";
        return;
    }

    if (beratKg > 999) {
        cout << "Berat sampah tidak boleh lebih besar dari 999 kg.\n";
        return;
    }

    // Cari jenis sampah di database
    int poinPerKg = 0;
    bool jenisDitemukan = false;
    for (const auto& item : database["sampah"]) {
        if (item["nama"].asString() == database["sampah"][jenisSampah-1]["nama"].asString()) {
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
    transaksiBaru["jenis_sampah"] = database["sampah"][jenisSampah-1]["nama"].asString();
    transaksiBaru["berat_kg"] = beratKg;
    transaksiBaru["poin_diterima"] = poinDiterima;
    transaksiBaru["tanggal"] = "2025-05-11"; // Tanggal bisa diubah sesuai kebutuhan
    transaksiBaru["username"] = username;
    transaksiBaru["petugas"] = currentUser.username;

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
    bool adaCatatanTransaksi = false;
    cout << "Lihat Catatan Transaksi\n";
    cout << "Catatan Transaksi:\n";
    cout << "╔═════════╦═════════════════════╦═══════════════╦═══════════════╦═══════════════╦════════════════╗\n";
    cout << "║ ID      ║ Username            ║ Tanggal       ║ Jenis Sampah  ║ Berat Sampah  ║ Poin Diterima  ║\n";
    cout << "╠═════════╬═════════════════════╬═══════════════╬═══════════════╬═══════════════╬════════════════╣\n";
    for (const auto& trx : database["transactions"]) {
        if (trx["petugas"].asString() != currentUser.username) {
            continue;
        }
        cout << "║ " << setw(8) << left << trx["id_transaksi"].asString()
             << "║ " << setw(20) << left << trx["username"].asString()
             << "║ " << setw(13) << left << trx["tanggal"].asString()
             << " ║ " << setw(13) << left << trx["jenis_sampah"].asString()
             << " ║ " << setw(12) << left << trx["berat_kg"].asDouble() << "kg"
             << "║ " << setw(15) << left << trx["poin_diterima"].asInt() << "║\n";
        adaCatatanTransaksi = true;
    }
    
    if (!adaCatatanTransaksi) {
        cout << "║                            Tidak ada catatan transaksi.                                        ║\n";
    }
    cout << "╚═════════╩═════════════════════╩═══════════════╩═══════════════╩═══════════════╩════════════════╝\n";
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
    cin.ignore();
    getline(cin, username);
    cout << "╠════════════════════════════════════════════╣\n";
    cout << "   Password: ";
    getline(cin, password);
    cout << "╚════════════════════════════════════════════╝\n";

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
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    cout << "Bak Sampah Digital 'Greencycle Program'" << endl;

    // Muat database dari file JSON
    loadDatabase();

    while (true) {
        int choice;
        cout << "\n" << endl;
        system("cls");
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

