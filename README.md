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






