# Praktikum Sistem Operasi Modul 3 - IT17

## Anggota Kelompok

| NRP        | Nama                            |
|:----------:|:-------------------------------:|
| 5027241006 | Nabilah Anindya Paramesti       |
| 5027241092 | Muhammad Khairul Yahya          |
| 5027241002 | Balqis Sani Sabillah            |


## Daftar Isi

- [Soal 1](#soal-1)
- [Soal 2](#soal-2)
- [Soal 3](#soal-3)
- [Soal 4](#soal-4)

# Soal 1
_**Oleh : Nabilah Anindya Paramesti**_

## Deskripsi Soal
Karena anda adalah seorang professional programmer, anda mengikuti petunjuk yang anda dapatkan dari deep web untuk membuat **sistem RPC server-client untuk mengubah text file sehingga bisa dilihat dalam bentuk file jpeg**.

## Jawaban
### A. Download dan Unzip file txt
Text file rahasia terdapat pada [LINK INI](https://drive.google.com/file/d/15mnXpYUimVP1F5Df7qd_Ahbjor3o1cVw/view?usp=sharing), diperbolehkan untuk download/unzip secara manual

### Penyelesaian A
aa

### B. Membuat image_client.c (daemon)
Pada image_server.c, program yang dibuat harus berjalan secara daemon di background dan terhubung dengan image_client.c melalui socket RPC.

### Penyelesaian B
aa

### C. Membuat `image_client.c` dan `image_server.c` terhubung
Program `image_client.c` harus bisa terhubung dengan `image_server.c` dan bisa mengirimkan perintah untuk:
- Decrypt text file yang dimasukkan dengan cara **Reverse Text lalu Decode from Hex**, untuk disimpan dalam folder database server dengan nama file berupa **timestamp** dalam bentuk angka, misalnya: `database/1744401282.jpeg`
- Request download dari database server sesuai filename yang dimasukkan, misalnya: `1744401282.jpeg`
- **Note: tidak diperbolehkan copy/pindah file, gunakan RPC untuk mengirim data.**

### Penyelesaian C
aa

### D. Membuat menu `image_client.c`
Program i`mage_client.c` harus disajikan dalam bentuk menu kreatif yang memperbolehkan pengguna untuk memasukkan perintah berkali-kali.

### Penyelesaian D
aa

### E. Dapat mengirim text file dan menerima file jpeg
Program dianggap berhasil bila pengguna dapat mengirimkan text file dan menerima sebuah file jpeg yang dapat dilihat isinya.

### Penyelesaian E
aa


### F. Menangani Error
Program `image_server.c` diharuskan untuk tidak keluar/terminate saat terjadi error dan client akan menerima error message sebagai response, yang meliputi minimal:
- Dari Client:
  - Gagal connect ke server
  - Salah nama text file input
- Dari Server:
  - Gagal menemukan file untuk dikirim ke client

### Penyelesaian F
aa

### G. Menyimpan ke log file
Server menyimpan log semua percakapan antara image_server.c dan image_client.c di dalam file server.log dengan format:
```
[Source][YYYY-MM-DD hh:mm:ss]: [ACTION] [Info]
# Contoh:
[Client][2025-03-12 02:23:17]: [DECRYPT] [Text data] 
[Server][2025-03-12 02:23:17]: [SAVE] [1744399397.jpeg] 
[Client][2025-03-12 02:23:26]: [DOWNLOAD] [1744399397.jpeg] 
[Server][2025-03-12 02:23:26]: [UPLOAD] [1744399397.jpeg] 
[Client][2025-03-12 02:23:31]: [DECRYPT] [Text data] 
[Server][2025-03-12 02:23:31]: [SAVE] [1744399411.jpeg] 
[Client][2025-03-12 02:23:38]: [DOWNLOAD] [1744399411.jpeg] 
[Server][2025-03-12 02:23:38]: [UPLOAD] [1744399411.jpeg] 
[Client][2025-03-12 02:23:39]: [EXIT] [Client requested to exit]
```

### Penyelesaian G
aa

# Soal 3
_**Oleh : Muhammad Khairul Yahya**_

## Deskripsi Soal 
Suatu pagi, anda menemukan jalan setapak yang ditumbuhi lumut dan hampir tertutup semak. Rasa penasaran membawamu mengikuti jalur itu, hingga akhirnya anda melihatnya: sebuah kastil tua, tertutup akar dan hampir runtuh, tersembunyi di tengah hutan. Gerbangnya terbuka seolah memanggilmu masuk.

Di dalam, anda menemukan pintu batu besar dengan simbol-simbol aneh yang terasa… hidup. Setelah mendorongnya dengan susah payah, anda pun menuruni tangga batu spiral yang dalam dan gelap. Di ujungnya, anda menemukan sebuah dunia baru: dungeon bawah tanah yang sudah tertinggal sangat lama.

Anda tidak tahu bagaimana anda dapat berada di situasi ini, tetapi didorong oleh rasa ingin tahu dan semangat, apa pun yang menunggumu di bawah sana, anda akan melawan.

### Cara Kerja Program???

**Lost Dungeon** menggunakan arsitektur klien-server berbasis soket TCP:

1. **Server (`dungeon.c`):**
- Berjalan pada `127.0.0.1:8888` dan menunggu koneksi dari klien.
- Menggunakan thread POSIX untuk melayani koneksi dari klien secara independen.
- Memproses perintah klien dan mengirim respons (`misalkan, untuk "1" untuk cek status pemain`).
- Menyimpan data pemain(gold, inventory, kills) dalam struktur `player` untuk setiap klien.

2. **Klien (`player.c`):**
- Terhubung ke server dan menampikan menu utama dengan opsi seperti cek status, beli senjata, atau battle.
- Mengirim perintah ke server melalui soket dan menampilkan respons server.

3. **Toko (`shop.c`):**
- Menyediakan daftar senjata dengan atribut (nama, harga, damage, passive).
- Menangani logika pembelian dan validasi (gold cukup, inventory penuh).

**Konsep Teknis:**
- **Threading:** Setiap klien ditangani dalam thread terpisah untuk mendukung multi-klien tanpa konflik data.

- **IPC/RPC:** Klien mengirim perintah ke server melalui soket, dan server memprosesnya seperti panggilan fungsi jarak jauh.

- **Warna ANSI:** Digunakan untuk meningkatkan estetika (hijau untuk bar hp, kuning untuk gold).

- **Error Handling:** Menangani input tidak valid dengan pesan yang jelas untuk pengalaman klien/user yang lebih baik.

### Soal 3-A:

**Entering the dungeon**

dungeon.c akan bekerja sebagai server yang dimana client (player.c) dapat terhubung melalui RPC. dungeon.c akan memproses segala perintah yang dikirim oleh player.c. Lebih dari 1 client dapat mengakses server.

### Jawaban 3-A
`dungeon.c` adalah server yang memungkinkan beberapa klien (`player.c`) terhubung menggunakan mekanisme RPC. Server harus menangani semua perintah klien dan mendukung lebih dari satu klien secara bersamaan.

**Penyelesaian:**

- **Soket TCP:** Server dibuat menggunakan soket TPC pada alamat `127.0.0.1:8888` dan port `8888`. Memungkinkan komunikasi 2 arah antara server & klien.

- **Thread POSIX:** Setiap klien ditangani dalam thread terpisah menggunakan `pthread_create` dan `pthread_detach`. Thread menjalankan fungsi `handle_client` untuk memproses perintah klien.

- **Stuktur `player`:** Setiap klien memiliki instance `player` yang menyimpan data seperti gold, weapon inventory, dan jumlah musuh yang dikalahkan.

- **Koneksi Klien:** `player.c` membuat soket, terhubung ke server, dan memulai interaksi dengan mengirim perintah (untuk cek status) dan menerima respons.

**Kode:**
```c
#define BUFFER_SIZE 2048
#define MAX_INVENTORY 10

typedef struct {
    char name[50];
    int price;
    int damage;
    char passive[100];
} Weapon;

typedef struct {
    int gold;
    Weapon inventory[MAX_INVENTORY];
    int inventory_count;
    int equipped_weapon_index;
    int kills;
} Player;

// dungeon.c
void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    free(arg);
    Player player = { .gold = 0, .inventory_count = 0, .kills = 0, .equipped_weapon_index = -1 };
    char buffer[BUFFER_SIZE];

    while (1) {
        int bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) break;
        buffer[bytes_received] = '\0';
        process_command(&player, buffer, client_sock);
    }
    close(client_sock);
    return NULL;
}

int main() {
    srand(time(NULL));
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = { .sin_family = AF_INET, .sin_port = htons(8888), .sin_addr.s_addr = INADDR_ANY };
    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sock, 10);
    printf("Server berjalan, menunggu koneksi...\n");

    while (1) {
        int *client_sock = malloc(sizeof(int));
        *client_sock = accept(server_sock, NULL, NULL);
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, client_sock);
        pthread_detach(thread);
    }
    close(server_sock);
    return 0;
}
```

### Penjelasan:

- **Soket TCP:** Fungsi `socket`, `bind`, `listen`, dan `accept` digunakan untuk membuat server yang mendengarkan koneksi klien. Port `8888` dipilih untuk komunikasi local.

- **Threading:** `pthread_create` membuat thread baru untuk setiap klien, dan `pthread_detach` memastikan thread dibersihkan otomatis setelah selesai. Guna mencegah kebocoran memori.

- **Struktur `Player`:** Menyimpan data pemain seperti `gold` (awalnya 0, ~~hardcore bgt~~), `inventory` (array senjata, maks 10), `inventory_count` (jumlah senjata), `equipped_weapon_index` (indeks senjata yang digunakan, -1 jika tidak ada), dan `kills` (jumlah enemy yang dibunuh/dikalahkan).

- **Fungsi `handle_client`:** Menerima perintah klien melalui `recv`, memprosesnya dengan `prosess_command`, dan mengirim respons dengan `send`. Jika koneksi terputus (`bytes_received <= 0`), thread menutup soket dan selesai.

- **Security/Keamanan:** Menggunakan `malloc` untuk menyimpan soket klien dan `free` untuk mencegah kebocoran memori. Menggunakan `pthread_detach` untuk membersihkan thread setelah selesai. Buffer dibatas dengan `BUFFER_SIZE`(2048) untuk mencegah ***overflow***.

### Tampilan Notifikasi Server

- **Untuk Notifikasi Server sedang berjalan:**
  ![image alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/59abbedfc0b216bfc57bc7d93e1d485384784ea4/assets/Screenshot%202025-05-08%20142309.png)

- **Saat ada klien yang menggunakan game Dungeon:**
  ![image alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/59abbedfc0b216bfc57bc7d93e1d485384784ea4/assets/Screenshot%202025-05-08%20142334.png)

- **Notifikasi server terhubung dari sudut pandang klien:**
  ![iamge alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/59abbedfc0b216bfc57bc7d93e1d485384784ea4/assets/Screenshot%202025-05-08%20142344.png)

- **Notifikasi jika klien (lebih dari 1) menggunakan game Dungeon:**
  ![image alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/59abbedfc0b216bfc57bc7d93e1d485384784ea4/assets/Screenshot%202025-05-08%20142411.png)

### Soal 3-B

**Sightseeing**

Anda melihat disekitar dungeon dan menemukan beberapa hal yang menarik seperti toko senjata dan pintu dengan aura yang cukup seram. Ketika player.c dijalankan, ia akan terhubung ke dungeon.c dan menampilkan sebuah main menu seperti yang dicontohkan di bawah ini (tidak harus mirip, dikreasikan sesuai kreatifitas masing-masing praktikan).

###  Jawaban 3-B

**Penyelesaian**
- **Menu Utama:** Ada ASCII art, pesan sambutan, dan 5 opsi:
1. Show Player Stats
2. Shop (Buy Weapons)
3. View Inventory & Equip Weapons
4. Battle Mode
5. Exit Game

- **Interaksi Klien:** Klien mengirim perintah ke server, menerima respons, dan menampilkannya. mensimulasikan RPC, di mana klien memanggil fungsi di server.

**Kode:**
```c
// player.c
void display_menu() {
    printf("\033[32m=====================================\n");
    printf("   SELAMAT DATANG DI LOST DUNGEON   \n");
    printf("=====================================\033[0m\n");
    printf("Kau berdiri di tengah dungeon kuno.\n");
    printf("Di dekatmu ada toko senjata dan pintu seram...\n\n");
    printf("\033[1m========== MAIN MENU ==========\033[0m\n");
    printf("  1. Show Player Stats\n");
    printf("  2. Shop (Buy Weapons)\n");
    printf("  3. View Inventory & Equip Weapons\n");
    printf("  4. Battle Mode\n");
    printf("  5. Exit Game\n\n");
    printf("Choose an option: ");
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = { .sin_family = AF_INET, .sin_port = htons(8888), .sin_addr.s_addr = inet_addr("127.0.0.1") };
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Gagal terhubung ke server");
        return 1;
    }
    printf("Terhubung ke server!\n");

    char buffer[BUFFER_SIZE];
    while (1) {
        display_menu();
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        send(sock, buffer, strlen(buffer), 0);
        int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) break;
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
        if (strcmp(buffer, "Koneksi terputus\n") == 0) break;
    }
    close(sock);
    return 0;
}
```

### Keterangan:
- **Menu Opsi:** Opsi 1-5 dicetak dengan teks bold (`/033[1m`). Pesan naratif ("Toko senjata dan pintu seram...")

- **Koneksi Socket:** Fungsi `socket` dan `connect` menghubungkan klien ke server. Jika gagal, `perror` menampilkan pesan error.

- **Pengiriman Perintah:** `fgets` mengambil input pengguna, `strcspn` menghapus newline, dan `send` mengirim perintah ke server. Respons diterima dengan `recv` dan ditampilkan.

- **Penutupan Koneksi:** Jika server mengirim "Koneksi terputus" atau koneksi gagal (`bytes_received <=0`), klien menutup socket dan keluar.

- **Safety/Keamanan:** Buffer dibatas dengan `BUFFER_SIZE` untuk mencegah overflow, dan input dibersihkan.

### Tampilan Main Menu:

![image alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/9150c4fcd443ee1b52bc9587a96f6fcd68e6038c/assets/Screenshot%202025-05-03%20132814.png)

### Soal 3-C:

**Status Check**

Melihat bahwa terdapat sebuah toko senjata, anda mengecek status diri anda dengan harapan anda masih memiliki sisa uang untuk membeli senjata. Jika opsi Show Player Stats dipilih, maka program akan menunjukan Uang yang dimiliki (Jumlah dibebaskan), senjata yang sedang digunakan, Base Damage, dan jumlah musuh yang telah dimusnahkan. 

### Jawaban 3-C

**Deskripsi:** Opsi "Show Player Stats" menampilkan jumlah gold, nama senjata yang digunakan, base damage, dan jumlah enemy yg dibunuh/dikalahkan. Jika senjata memiliki efek pasif, deskripsinya juga ditampilkan.

**Penyelesaian:**

- **Status Pemain:** Ditampilkan dalam format tabel denagn warna ANSI untuk hiasan

- **Default:** Jika tidak ada senjata yang digunakan, senjata default adalah "Fists" dengan base damage 5. Gold dan kills dimulai dari 0 (~~kayak pom bensin~~).

**Kode:**
```c
// dungeon.c
void show_player_stats(Player *player, char *buffer) {
    Weapon *weapon = player->equipped_weapon_index >= 0 ? &player->inventory[player->equipped_weapon_index] : NULL;
    snprintf(buffer, BUFFER_SIZE, 
             "\033[1m======== PLAYER STATS ========\033[0m\n"
             "GOLD: \033[33m%d\033[0m  |  EQUIPPED WEAPON: %s  |  BASE DAMAGE: %d  |  KILLS: %d\n",
             player->gold, 
             weapon ? weapon->name : "Fists", 
             weapon ? weapon->damage : 5, 
             player->kills);
    if (weapon && strlen(weapon->passive) > 0) {
        char temp[BUFFER_SIZE];
        snprintf(temp, BUFFER_SIZE, "%sPASSIVE: \033[36m%s\033[0m\n", buffer, weapon->passive);
        strcpy(buffer, temp);
    }
}
```
### Keterangan:

- **Senjata Default:** jika `equipped_weapon_index < 0`, "Fists" digunakan dengan base damage 5, mencerminkan kondisi default/awal pemain.

- **Efek Pasif:** hanya ditampilkan jika senjata memiliki pasif (`strlen(weapon->passive) > 0`). Ini menangani kasus seperti "Pedang Lorem Ipsum" yang tidak memiliki pasif.

- **Security/Keamanan:** buffer sementara (`temp`) digunakan untuk menambahkan pasif, memastikan string tidak terpotong.

- **Fleksibilitas:** Fungsi ini dapat diperluas untuk menampilkan statistik lain (HP Pemain) jika diperlukan.

### Menu Status Check untuk Opsi 1:

![image alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/9150c4fcd443ee1b52bc9587a96f6fcd68e6038c/assets/Screenshot%202025-05-08%20142439.png)

- Ini merupakan tampilan awal pemain sebelum grinding gold.

### Soal 3-D:

**Weapon Shop**

Ternyata anda memiliki sisa uang dan langsung pergi ke toko senjata tersebut untuk membeli senjata. Terdapat 5 pilihan senjata di toko tersebut dan beberapa dari mereka memiliki passive yang unik. Disaat opsi Shop dipilih, program akan menunjukan senjata apa saja yang dapat dibeli beserta harga, damage, dan juga passive (jika ada). List senjata yang ada dan dapat dibeli beserta logic/command untuk membeli senjata tersebut diletakan di code shop.c/shop.h yang nanti akan dipakai oleh dungeon.c.

### Jawaban 3-D

**Deskripsi:** Opsi "shot" menampilkan minimal 5 senjata dengan nama, harga, damage, dan pasif (minimal 2 senjata dengan pasif). Logic toko diimplementasikan di `shop.c`. Pemain dapat membeli senjata jika gold cukup.

**Penyelesaian:**

- **Daftar Senjata:** `shop.c` mendefinisikan 10 senjata, 5 diantaranya memiliki pasif. Seperti contoh berikut:

1. **Pedang Lorem Ipsum:** 100 Gold, 10 Damage

2. **AK47:** 500 Gold, 20 Damage, +30% Crit Chance

3. **Es Krim Choco Mint:** 300 Gold, 15 Damage, +45% Poisoned

4. **Keris Hitam Pujo:** 400 Gold, 18 Damage, +15% Insta-Kill Chance

5. **Tongkat Cherybelle:** 600 Gold, 25 Damage, +58% Stun Chance

...**dan lain sebagainya**

- **Pembelian:** Pemain/klien/user memasukkan nomor senjata (1-10, 0 untuk cancel). Server memvalidasi gold dan kapasitas inventaris sebelum menambah senjata.

- **Kode:**
```c
// shop.h
typedef struct {
    char name[50];
    int price;
    int damage;
    char passive[100];
} Weapon;

// shop.c
Weapon shop[] = {
    {"Pedang Lorem Ipsum", 100, 10, ""},
    {"AK47", 500, 20, "+30% Crit Chance"},
    {"Es Krim Choco Mint", 300, 15, "+45% Poisoned"},
    {"Keris Hitam Pujo", 400, 18, "+15% Insta-Kill Chance"},
    {"Tongkat Cherybelle", 600, 25, "+58% Stun Chance"},
    {"Katana", 200, 12, ""},
    {"Busur Naga", 350, 17, ""},
    {"Kapak Perang", 450, 22, ""},
    {"Cambuk Api", 550, 23, "+20% Burn Chance"},
    {"Palu Thor", 700, 30, ""}
};

void display_shop(char *buffer) {
    snprintf(buffer, BUFFER_SIZE, "\033[1m======== WEAPON SHOP ========\033[0m\n");
    for (int i = 0; i < 10; i++) {
        char temp[BUFFER_SIZE];
        snprintf(temp, BUFFER_SIZE, "%s%d. %s - \033[33m%d Gold\033[0m, \033[31m%d Damage\033[0m%s\n",
                 buffer, i + 1, shop[i].name, shop[i].price, shop[i].damage,
                 shop[i].passive[0] ? ", \033[36mPassive: " : "");
        strcpy(buffer, temp);
        if (shop[i].passive[0]) {
            snprintf(temp, BUFFER_SIZE, "%s%s\033[0m\n", buffer, shop[i].passive);
            strcpy(buffer, temp);
        }
    }
    strcat(buffer, "\nEnter weapon number (1-10, 0 to cancel): ");
}

int buy_weapon(Player *player, int choice) {
    if (choice < 1 || choice > 10) return 0;
    Weapon *weapon = &shop[choice - 1];
    if (player->gold < weapon->price) return -1;
    if (player->inventory_count >= MAX_INVENTORY) return -2;
    player->gold -= weapon->price;
    player->inventory[player->inventory_count++] = *weapon;
    return 1;
}
```
### Keterangan:

- **Struktur `weapon`:** Menyimpan `name` (string), `price`(int), `damage`(int), dan `passive`(string). Pasif koosng (`""`) untuk senjata tanpa efek.

- **Fungsi `display_shop`:** Menggunakan `snprintf` untuk memformat daftar senjata dengan aman. warna kuning(`\033[33m`) = harga, merah(`\033[31m`) = damage, cyan(`\033[36m`) = pasif.

- **Fungsi `buy_weapon`:** Memvalidasi input (`choice` antara 1-10), memeriksa gold (`player->gold >= weapon->price`), dan kapasitas inventaris (`inventory_count < 10`).
Mengembalikan:

1. `1` jika pembelian berhasil.
2. `0` jika pilihan tidak valid.
3. `-1` jika gold tidak cukup.
4. `-2` jika inventaris penuh,.

- **Keamanan/Security:** Menggunakan `strcpy` dan `strcat` dengan buffer sementara untuk mencegah overflow. Array `shop` didefinisikan secara statis untuk efisiensi.

### Tampilan Shop:

- Ini merupakan list-list senjata yang dijual di **Weapon Shop:**

  ![image alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/9150c4fcd443ee1b52bc9587a96f6fcd68e6038c/assets/Screenshot%202025-05-08%20142529.png)

### Soal 3-E

**Handy Inventory**

Setelah membeli senjata di toko tadi, anda membuka ransel anda untuk memakai senjata tersebut. Jika opsi View Inventory dipilih, program akan menunjukan senjata apa saja yang dimiliki dan dapat dipakai (jika senjata memiliki passive, tunjukan juga passive tersebut).

Lalu apabila opsi Show Player Stats dipilih saat menggunakan weapon maka Base Damage player akan berubah dan jika memiliki passive, maka akan ada status tambahan yaitu Passive.


### Jawaban 3-E

**Deskripsi:** Opsi "View Inventory" menampilkan daftar senjata yang dimiliki pemain, termasuk nama, damage, dan pasif (jika ada). Pemain dapat memilih senjata untuk digunakan, dan opsi "Show Player Stats" mencerminkan base damage serta pasif senjata yang aktif.

**Penyelesaian:**

- **Tampilan Inventaris:** Menampilkan senjata dengan indeks (1–n), nama, damage, dan pasif. Senjata yang sedang digunakan ditandai dengan tanda bintang (*).

- **Equip Senjata:** Pemain memasukkan indeks senjata untuk mengaktifkannya, memperbarui `equipped_weapon_index.` Input kosong (Enter) kembali ke menu utama.

- **Integrasi dengan Status:** Opsi "Show Player Stats" menampilkan base damage dan pasif dari senjata yang sedang digunakan.

**Kode:**
```c
// dungeon.c
void view_inventory(Player *player, char *buffer) {
    snprintf(buffer, BUFFER_SIZE, "\033[1m======== INVENTORY ========\033[0m\n");
    if (player->inventory_count == 0) {
        strcat(buffer, "Inventory kosong!\n");
    } else {
        for (int i = 0; i < player->inventory_count; i++) {
            char temp[BUFFER_SIZE];
            snprintf(temp, BUFFER_SIZE, "%s[%d] %s - Damage: \033[31m%d\033[0m%s%s\n",
                     buffer, i + 1, player->inventory[i].name, player->inventory[i].damage,
                     player->inventory[i].passive[0] ? ", Passive: \033[36m" : "",
                     player->inventory[i].passive[0] ? player->inventory[i].passive : "");
            strcpy(buffer, temp);
            if (i == player->equipped_weapon_index) {
                strcat(buffer, " \033[1m(*)\033[0m");
            }
            strcat(buffer, "\n");
        }
    }
    strcat(buffer, "\nEnter weapon number to equip (or Enter to return): ");
}

void equip_weapon(Player *player, int choice, char *buffer) {
    if (choice < 1 || choice > player->inventory_count) {
        snprintf(buffer, BUFFER_SIZE, "Pilihan tidak valid!\n");
    } else {
        player->equipped_weapon_index = choice - 1;
        snprintf(buffer, BUFFER_SIZE, "Equipped %s!\n", player->inventory[choice - 1].name);
    }
}
```

### Keterangan:

- **Fungsi `view_inventory`:** Menampilkan inventaris dengan format:
```plain
[1] Pedang Lorem Ipsum - Damage: 10
[2] AK47 - Damage: 20, Passive: +30% Crit Chance (*)
```
Tanda bintang (*) menunjukkan senjata aktif. Warna merah untuk damage dan cyan untuk pasif.

- **Fungsi `equip_weapon`:** Memvalidasi input (`choice` antara 1 dan `inventory_count`). Jika valid, memperbarui `equipped_weapon_index` dan mengirim pesan konfirmasi.

- **Inventory Kosong:** Jika `inventory_count == 0`, menampilkan "Inventory kosong!" untuk kejelasan.

- **Integrasi:** `equipped_weapon_index` digunakan oleh `show_player_stats` untuk menampilkan damage dan pasif senjata aktif.

- **Keamanan/Security:** Menggunakan `snprintf` dan buffer sementara untuk mencegah overflow. Input kosong ditangani dengan kembali ke menu utama.

### Tampilan Inventory Player:

![image alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/9150c4fcd443ee1b52bc9587a96f6fcd68e6038c/assets/Screenshot%202025-05-08%20142742.png)

- **Penjelasan:**
1. Masukkan angka untuk menequip senjata yang ada di dalam inventory player.
2. klik enter untuk kembali ke menu.

- **Jika yang diequip memiliki passive, maka tampilannya akan seperti ini:**
- 
  ![image alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/9150c4fcd443ee1b52bc9587a96f6fcd68e6038c/assets/Screenshot%202025-05-08%20142804.png)
  
### Soal 3-F

**Enemy Encounter**

Anda sekarang sudah siap untuk melewati pintu yang seram tadi, disaat anda memasuki pintu tersebut, anda langsung ditemui oleh sebuah musuh yang bukan sebuah manusia. Dengan tekad yang bulat, anda melawan musuh tersebut. Saat opsi Battle Mode dipilih, program akan menunjukan health-bar musuh serta angka yang menunjukan berapa darah musuh tersebut dan menunggu input dengan opsi attack untuk melakukan sebuah serangan dan juga exit untuk keluar dari Battle Mode. Apabila darah musuh berkurang, maka health-bar musuh akan berkurang juga.

Jika darah musuh sudah 0, maka program akan menunjukan rewards berupa berapa banyak gold yang didapatkan lalu akan muncul musuh lagi.

### Jawaban 3-F:

**Deskripsi:** Opsi "Battle Mode" menampilkan bilah kesehatan musuh, angka HP, dan menerima input `attack` atau `exit`. Bilah kesehatan berkurang seiring penurunan HP. Jika musuh kalah (HP ≤ 0), pemain mendapatkan gold acak, jumlah kills bertambah, dan musuh baru muncul.

**Penyelesaian:**

- **Health Bar:** Ditampilkan dengan karakter █ (panjang 20), berwarna hijau (>50%), kuning (>30%), atau merah (≤30%). Persentase HP juga ditampilkan.

- **Input:** `attack` memicu serangan, `exit` kembali ke menu utama. Input lain dianggap tidak valid.

- **Musuh:** HP awal acak (50–200). Jika kalah, memberikan gold acak (50–149) dan diganti dengan musuh baru.

- **Tampilan:** Contoh:
```plain
Enemy HP: 75/100
[██████████████      ] 75%
Type 'attack' to attack or 'exit' to leave:
```

- **Kode:**
```c
// dungeon.h
typedef struct {
    int hp;
    int max_hp;
    int poisoned_turns;
    int stunned;
} Enemy;

// dungeon.c
void battle_mode(Player *player, char *buffer, const char *command) {
    static Enemy enemy = {0};
    if (enemy.hp <= 0 || strcmp(command, "battle") == 0) {
        enemy.hp = enemy.max_hp = 50 + rand() % 151;
        enemy.poisoned_turns = 0;
        enemy.stunned = 0;
    }
    if (strcmp(command, "attack") == 0) {
        if (enemy.stunned) {
            enemy.stunned = 0;
            strcat(buffer, "Enemy is stunned and skips turn!\n");
        } else {
            int damage = (player->equipped_weapon_index >= 0 ? player->inventory[player->equipped_weapon_index].damage : 5) + (rand() % 10);
            if (rand() % 100 < 10) damage *= 2; // 10% crit chance
            apply_passive(player, &enemy, buffer);
            enemy.hp -= damage;
            snprintf(buffer, BUFFER_SIZE, "%sYou deal \033[31m%d damage\033[0m!\n", buffer, damage);
            if (enemy.hp <= 0) {
                int reward = 50 + rand() % 100;
                player->gold += reward;
                player->kills++;
                snprintf(buffer, BUFFER_SIZE, "%sEnemy defeated! You gained \033[33m%d gold\033[0m!\n", buffer, reward);
                enemy.hp = enemy.max_hp = 50 + rand() % 151;
            }
        }
    } else if (strcmp(command, "exit") != 0 && strcmp(command, "battle") != 0) {
        snprintf(buffer, BUFFER_SIZE, "Pilihan tidak valid!\n");
        return;
    }
    char health_bar[21] = {0};
    int bar_length = (enemy.hp * 20) / enemy.max_hp;
    for (int i = 0; i < bar_length; i++) health_bar[i] = '█';
    const char *color = enemy.hp > enemy.max_hp * 0.5 ? "\033[32m" : enemy.hp > enemy.max_hp * 0.3 ? "\033[33m" : "\033[31m";
    snprintf(buffer, BUFFER_SIZE, "%sEnemy HP: %d/%d\n%s[%s%-20s\033[0m] %d%%\nType 'attack' to attack or 'exit' to leave: ",
             buffer, enemy.hp, enemy.max_hp, color, health_bar, "", (enemy.hp * 100) / enemy.max_hp);
}
```

### Keterangan:

- **Struktur `Enemy`:** Menyimpan `hp` (kesehatan saat ini), `max_hp` (kesehatan maksimum), `poisoned_turns` (sisa giliran poison), dan `stunned` (status stun).

- **Health Bar:** Panjang bar dihitung sebagai `(hp * 20) / max_hp`. Warna ditentukan berdasarkan persentase HP: hijau (`\033[32m`), kuning (`\033[33m`), merah (`\033[31m`).

- **Serangan:** Damage dihitung sebagai `base_damage + (rand() % 10)`, dengan 10% peluang kritikal (2x damage). Pasif senjata diterapkan melalui `apply_passive`.

- **Musuh Baru:** Jika `hp <= 0`, musuh diganti dengan HP acak baru, dan pemain menerima gold (`50 + rand() % 100`).

- **Input Handling:** Hanya `attack` dan `exit` yang valid. Input lain memicu pesan error.

- **Keamanan/Security:** Menggunakan `snprintf` untuk memformat string, dan `static Enemy` memastikan musuh tetap ada selama sesi pertempuran.

### Contoh tampilan battle dimulai:

- **Contoh gameplay untuk Battle Mode:**

![image alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/9150c4fcd443ee1b52bc9587a96f6fcd68e6038c/assets/Screenshot%202025-05-08%20142834.png)

**Penjelasan:**

1. Untuk attack akan memunculkan berapa damage yang diterima oleh musuh.
2. Untuk senjata yang memiliki pasif maka akan ada pesan yang mengindikasikan passive nya aktif saat command menyerah dari user/klien.
3. Ketika sudah membunuh/memenangkan pertarungan, maka akan ada reward berupa gold.
4. Namun saat selesai membunuh/memenangkan 1 pertarungan, maka musuh akan muncul lagi.

### Soal 3-G

**Other Battle Logic**

- **Health & Rewards**
Untuk darah musuh, seberapa banyak darah yang mereka punya dibuat secara random, contoh: 50-200 HP. Lakukan hal yang sama untuk rewards. 
- **Damage Equation**
Untuk damage, gunakan base damage sebagai kerangka awal dan tambahkan rumus damage apapun (dibebaskan, yang pasti perlu random number agar hasil damage bervariasi). Lalu buatlah logic agar setiap serangan memiliki kesempatan untuk Critical yang membuat damage anda 2x lebih besar.
- **Passive**
Jika senjata yang dipakai memiliki Passive setiap kali passive tersebut menyala, maka tunjukan bahwa passive tersebut aktif.

### Jawaban 3-G

**Deskripsi:**

- **Health & Reward:** HP musuh (50–200) dan hadiah gold (50–149) dihasilkan secara acak.

- **Persamaan Damage:** Base damage + bonus acak, dengan peluang kritikal (2x damage).

- **Pasif:** Efek pasif senjata (mis., crit chance, poison, stun) ditampilkan saat terpicu dengan pesan khusus.

**Penyelesaian:**

- **HP Musuh:** Dihasilkan dengan `50 + rand() % 151`, memberikan rentang 50–200.

- **Hadiah Gold:** Dihasilkan dengan `50 + rand() % 100`, memberikan rentang 50–149.

- **Damage:** `base_damage + (rand() % 10)` dengan peluang critical 10% (atau sesuai pasif, mis., +30% untuk AK47).

- **Pasif:** Contoh efek:

1. **Crit Chance (+30%):** Meningkatkan peluang kritikal menjadi 40% (10% default + 30%).
2. **Poison (+45%):** 45% peluang memberikan 5 damage per giliran selama 3 giliran.
3. **Stun (+58%):** 58% peluang membuat musuh melewati giliran.
3. **Insta-Kill (+15%):** 15% peluang membunuh musuh langsung.

- **Pesan saat pasif (unik):** Setiap pasif memiliki pesan unik, misalnya, "CHOCO-MINTO:3 BURIMO ana-taaa!!" untuk poison dari Es Krim Choco Mint.

- **Kode:**

```c
// dungeon.c
void apply_passive(Player *player, Enemy *enemy, char *buffer) {
    if (player->equipped_weapon_index < 0) return;
    Weapon *weapon = &player->inventory[player->equipped_weapon_index];
    if (strcmp(weapon->passive, "+30% Crit Chance") == 0 && rand() % 100 < 30) {
        strcat(buffer, "CRITICAL HIT! Damage doubled!\n");
    } else if (strcmp(weapon->passive, "+45% Poisoned") == 0 && rand() % 100 < 45) {
        enemy->poisoned_turns = 3;
        strcat(buffer, "CHOCO-MINTO:3 BURIMO ana-taaa!! Enemy poisoned for 3 turns!\n");
    } else if (strcmp(weapon->passive, "+58% Stun Chance") == 0 && rand() % 100 < 58) {
        enemy->stunned = 1;
        strcat(buffer, "Enemy stunned by Cherybelle's magic!\n");
    } else if (strcmp(weapon->passive, "+15% Insta-Kill Chance") == 0 && rand() % 100 < 15) {
        enemy->hp = 0;
        strcat(buffer, "INSTA-KILL! Enemy slain by Keris Hitam Pujo!\n");
    }
    if (enemy->poisoned_turns > 0) {
        enemy->hp -= 5;
        enemy->poisoned_turns--;
        strcat(buffer, "Poison deals \033[31m5 damage\033[0m!\n");
    }
}
```
### Keterangan:

- **Fungsi `apply_passive`:** Memeriksa pasif senjata dan menerapkannya berdasarkan peluang acak (`rand() % 100`).

- **Critical:** Default 10%, ditambah sesuai pasif (mis., +30% untuk AK47). Ditandai dengan pesan "CRITICAL HIT!".

- **Poison:** Mengatur `poisoned_turns = 3` dan mengurangi 5 HP per giliran. 

- **Stun:** Mengatur `stunned = 1`, mencegah musuh menyerang di giliran berikutnya.

- **Insta-Kill:** Mengatur `hp = 0` untuk instant-win!.

- **Keamanan/Security:** Menggunakan `strcmp` untuk membandingkan pasif dan `strcat` dengan buffer yang cukup besar.

- **Randomisasi:** `srand(time(NULL))` diinisialisasi di `main` untuk memastikan hasil acak berbeda setiap kali program dijalankan.

### Tampilan Battle untuk Logic-nya:

- **Untuk Damage Equation:**
  ![image alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/9150c4fcd443ee1b52bc9587a96f6fcd68e6038c/assets/Screenshot%202025-05-08%20142906.png)

- **Untuk efek passive dan pesannya:**

**1. Efek pasif poisoned dari Keris Hitam Pujo:**

![image alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/9150c4fcd443ee1b52bc9587a96f6fcd68e6038c/assets/Screenshot%202025-05-08%20142920.png)

**2. Efek instant kill dari AK47:**

![image alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/9150c4fcd443ee1b52bc9587a96f6fcd68e6038c/assets/Screenshot%202025-05-08%20142953.png)

**3. Efek menambah Critical Damage dari Es Krim Choco Minto:3**

![image alt](https://github.com/ninduyi/Sisop-3-2025-IT17/blob/9150c4fcd443ee1b52bc9587a96f6fcd68e6038c/assets/Screenshot%202025-05-08%20143019.png)
  
### Soal 3-H

**Error Handling**

Berikan error handling untuk opsi-opsi yang tidak ada.

### Jawaban 3-H

**Deskripsi:**
Program harus menangani opsi tidak valid dengan pesan kesalahan yang jelas untuk meningkatkan pengalaman pengguna.

**Penyelesaian:**

- **Menu Utama:** Input selain 1–5 menampilkan "Pilihan tidak valid. Silakan coba lagi."

- **Toko:** Nomor senjata <1, >10, atau 0 (batal) ditangani. Gold tidak cukup atau inventaris penuh menampilkan pesan spesifik.

- **Inventory:** Indeks senjata tidak valid menampilkan "Pilihan tidak valid!".

- **Battle:** Input selain `attack` atau `exit` menampilkan "Pilihan tidak valid!".

- **Prompt Ulang:** Setelah pesan error, pemain/klien/user diminta memasukkan input lagi.

**Kode:**

```c
// dungeon.c
void process_command(Player *player, char *command, int client_sock) {
    char buffer[BUFFER_SIZE] = {0};
    if (strcmp(command, "1") == 0) {
        show_player_stats(player, buffer);
    } else if (strcmp(command, "2") == 0) {
        display_shop(buffer);
    } else if (strcmp(command, "3") == 0) {
        view_inventory(player, buffer);
    } else if (strcmp(command, "4") == 0) {
        battle_mode(player, buffer, "battle");
    } else if (strcmp(command, "5") == 0) {
        snprintf(buffer, BUFFER_SIZE, "Koneksi terputus\n");
        send(client_sock, buffer, strlen(buffer), 0);
        return;
    } else {
        snprintf(buffer, BUFFER_SIZE, "Pilihan tidak valid. Silakan coba lagi.\n");
    }
    send(client_sock, buffer, strlen(buffer), 0);
}
```
### Keterangan:

- **Fungsi `process_command:`** Memeriksa perintah klien dan memanggil fungsi yang sesuai (mis., show_player_stats` untuk "1"). Jika perintah tidak valid, mengirim pesan error.

- **Pesan Error:** "Pilihan tidak valid!" untuk input di luar rentang.

- **Konteks-Spesifik:** Di toko, pesan seperti "Gold tidak cukup!" atau "Inventaris penuh!" memberikan umpan balik spesifik.

- **Keamanan/Security:** Menggunakan `snprintf` untuk memformat pesan error, memastikan tidak ada buffer overflow.

- **Pengalaman Pengguna:** Prompt ulang setelah error memungkinkan pemain memperbaiki input tanpa crash.

### Dokumentasi Error Handling:



# Soal 4
_**Oleh : Nabilah Anindya Paramesti**_

## Deskripsi Soal
Sung Jin Woo mengalami reinkarnasi dan sekarang bekerja sebagai seorang admin. Uniknya, pekerjaan ini mempunyai sebuah sistem yang bisa melakukan tracking pada seluruh aktivitas dan keadaan seseorang. Sayangnya, model yang diberikan oleh Bos-nya sudah sangat tua sehingga program tersebut harus dimodifikasi agar tidak ketinggalan zaman, dengan spesifikasi

## Jawaban
### A. Membuat `system.c` dan `hunter.c`
Agar hunter lain tidak bingung, Sung Jin Woo memutuskan untuk membuat dua file, yaitu `system.c` dan `hunter.c`. Sung Jin Woo mendapat peringatan bahwa `system.c` merupakan shared memory utama yang mengelola shared memory hunter-hunter dari `hunter.c`. Untuk mempermudahkan pekerjaannya, Sung Jin Woo mendapat sebuah [clue](https://drive.google.com/file/d/1E3tRDQ4xkSzs3sY3GT8gKb77iEfhOvZs/view?usp=sharing) yang dapat membuat pekerjaannya menjadi lebih mudah dan efisien. 

**NOTE : hunter bisa dijalankan ketika sistem sudah dijalankan.**

### Penyelesaian A

### B. Menambah fitur registrasi dan login di program hunter
Untuk memastikan keteraturan sistem, Sung Jin Woo memutuskan untuk membuat fitur registrasi dan login di program hunter. Setiap hunter akan memiliki key unik dan stats awal (Level=1, EXP=0, ATK=10, HP=100, DEF=5). Data hunter disimpan dalam shared memory tersendiri yang terhubung dengan sistem.

### Penyelesaian B

### C. Fitur yang menampilkan informasi semua hunter (sistem)
Agar dapat memantau perkembangan para hunter dengan mudah, Sung Jin Woo menambahkan fitur di sistem yang dapat menampilkan informasi semua hunter yang terdaftar, termasuk nama hunter, level, exp, atk, hp, def, dan status (banned atau tidak). Ini membuat dia dapat melihat siapa hunter terkuat dan siapa yang mungkin melakukan kecurangan.

### Penyelesaian C

### D. Fitur dalam sistem yang bisa menghasilkan dungeon secara random
Setelah beberapa hari bekerja, Sung Jin Woo menyadari bahwa para hunter membutuhkan tempat untuk berlatih dan memperoleh pengalaman. Ia memutuskan untuk membuat fitur unik dalam sistem yang dapat menghasilkan dungeon secara random dengan nama, level minimal hunter, dan stat rewards dengan nilai:
- 🏆Level Minimal : 1 - 5
- ⚔️ATK : 100 - 150 Poin
- ❤️HP  : 50 - 100 Poin
- 🛡️DEF : 25 - 50 Poin
- 🌟EXP : 150 - 300 Poin

Setiap dungeon akan disimpan dalam shared memory sendiri yang berbeda dan dapat diakses oleh hunter.

### Penyelesaian D

### E. Fitur menampilkan daftar lengkap dungeon
  Untuk memudahkan admin dalam memantau dungeon yang muncul, Sung Jin Woo menambahkan fitur yang menampilkan informasi detail semua dungeon. Fitur ini menampilkan daftar lengkap dungeon beserta nama, level minimum, reward (EXP, ATK, HP, DEF), dan key unik untuk masing-masing dungeon.

### Penyelesaian E

### F. Fitur yang menampilkan dungeon yang tersedia sesuai dengan level hunter.
Pada saat yang sama, dungeon yang dibuat oleh sistem juga harus dapat diakses oleh hunter. Sung Jin Woo menambahkan fitur yang menampilkan semua dungeon yang tersedia sesuai dengan level hunter. Disini, hunter hanya dapat menampilkan dungeon dengan level minimum yang sesuai dengan level mereka.

### Penyelesaian F

### G. Fitur raid dungeon
Setelah melihat beberapa hunter yang terlalu kuat, Sung Jin Woo memutuskan untuk menambahkan fitur untuk menguasai dungeon. Ketika hunter berhasil menaklukan sebuah dungeon, dungeon tersebut akan menghilang dari sistem dan hunter akan mendapatkan stat rewards dari dungeon. Jika exp hunter mencapai 500, mereka akan naik level dan exp kembali ke 0.

### Penyelesaian G
  
### H.Fitur battle hunter
Karena persaingan antar hunter semakin ketat, Sung Jin Woo mengimplementasikan fitur dimana hunter dapat memilih untuk bertarung dengan hunter lain. Tingkat kekuatan seorang hunter bisa dihitung melalui total stats yang dimiliki hunter tersebut (ATK+HP+DEF). Jika hunter menang, maka hunter tersebut akan mendapatkan semua stats milik lawan dan lawannya akan terhapus dari sistem. Jika kalah, hunter tersebutlah yang akan dihapus dari sistem dan semua statsnya akan diberikan kepada hunter yang dilawannya.

### Penyelesaian H

### I. Fitur ban hunter
Saat sedang memonitoring sistem, Sung Jin Woo melihat beberapa hunter melakukan kecurangan di dalam sistem. Ia menambahkan fitur di sistem yang membuat dia dapat melarang hunter tertentu untuk melakukan raid atau battle. Karena masa percobaan tak bisa berlangsung selamanya 😇, Sung Jin Woo pun juga menambahkan konfigurasi agar fiturnya dapat memperbolehkan hunter itu melakukan raid atau battle lagi. 

### Penyelesaian I

### J. Fitur reset hunter
Setelah beberapa pertimbangan, untuk memberikan kesempatan kedua bagi hunter yang ingin bertobat dan memulai dari awal, Sung Jin Woo juga menambahkan fitur di sistem yang membuat dia bisa mengembalikan stats hunter tertentu ke nilai awal. 

### Penyelesaian J

### K. Fitur notifikasi
Untuk membuat sistem lebih menarik dan tidak membosankan, Sung Jin Woo menambahkan fitur notifikasi dungeon di setiap hunter. Saat diaktifkan, akan muncul informasi tentang semua dungeon yang terbuka dan akan terus berganti setiap 3 detik.

### Penyelesaian K
  
### L. Konfigurasi Exit
Terakhir, untuk menambah keamanan sistem agar data hunter tidak bocor, Sung Jin Woo melakukan konfigurasi agar setiap kali sistem dimatikan, maka semua shared memory yang sedang berjalan juga akan ikut terhapus. 

### Penyelesaian L






