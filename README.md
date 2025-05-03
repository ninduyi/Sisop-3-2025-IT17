# Praktikum Sistem Operasi Modul 1 - IT17

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


## Jawaban
### A.