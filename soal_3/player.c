#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 2048 // Meningkatkan ukuran buffer agar sesuai dengan dungeon.c

int main() {
    int sock;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];
    int read_size;

    // Buat socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Gagal membuat socket");
        exit(1);
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    // Hubungkan ke server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Koneksi gagal");
        exit(1);
    }

    printf("Terhubung ke server!\n");

    // Loop utama untuk komunikasi dengan server
    while (1) {
        // Terima pesan dari server
        read_size = recv(sock, buffer, BUFFER_SIZE, 0);
        if (read_size <= 0) {
            printf("Koneksi terputus\n");
            break;
        }
        buffer[read_size] = '\0';
        printf("%s", buffer);

        // Jika pesan terakhir adalah perintah untuk keluar
        if (strstr(buffer, "Sampai jumpa!")) {
            break;
        }

        // Baca input pengguna
        char input[100];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error membaca input\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0'; // Hapus newline

        // Kirim input ke server
        if (send(sock, input, strlen(input) + 1, 0) < 0) {
            perror("Gagal mengirim pesan");
            break;
        }
    }

    close(sock);
    return 0;
}