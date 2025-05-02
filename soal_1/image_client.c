#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <zip.h>

#define PORT 8080
#define MAX_BUFFER 4096
#define ZIP_URL "https://drive.google.com/uc?export=download&id=15mnXpYUimVP1F5Df7qd_Ahbjor3o1cVw"
#define TEMP_ZIP "temp.zip"

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

void download_file(const char *url, const char *out_filename) {
    CURL *curl;
    FILE *fp;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(out_filename, "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Download failed: %s\n", curl_easy_strerror(res));
        }
        fclose(fp);
        curl_easy_cleanup(curl);
    }
}

void extract_zip(const char *zip_filename, const char *target_folder) {
    int err = 0;
    zip_t *z = zip_open(zip_filename, 0, &err);
    if (z == NULL) return;

    zip_int64_t num_entries = zip_get_num_entries(z, 0);
    for (zip_uint64_t i = 0; i < num_entries; i++) {
        zip_stat_t st;
        zip_stat_index(z, i, 0, &st);

        zip_file_t *zf = zip_fopen_index(z, i, 0);
        if (!zf) continue;

        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s/%s", target_folder, st.name);

        for (char *p = filepath + strlen(target_folder) + 1; *p; p++) {
            if (*p == '/') {
                *p = '\0';
                mkdir(filepath, 0777);
                *p = '/';
            }
        }

        if (st.name[strlen(st.name) - 1] == '/') {
            mkdir(filepath, 0777);
            zip_fclose(zf);
            continue;
        }

        FILE *fout = fopen(filepath, "wb");
        if (fout) {
            char buffer[4096];
            zip_int64_t n;
            while ((n = zip_fread(zf, buffer, sizeof(buffer))) > 0) {
                fwrite(buffer, 1, n, fout);
            }
            fclose(fout);
        }

        zip_fclose(zf);
    }

    zip_close(z);
}

void download_zip() {
    download_file(ZIP_URL, TEMP_ZIP);
    extract_zip(TEMP_ZIP, "client");
    remove(TEMP_ZIP);
}

void print_menu() {
    printf("\n================================\n");
    printf("    Image Decoder Client        \n");
    printf("================================\n");
    printf("1. Send input file to server\n");
    printf("2. Download file from server\n");
    printf("3. Exit\n");
    printf("================================\n");
}

int main() {
    struct stat st = {0};
    if (stat("client/secrets", &st) == -1) {
        download_zip();
    }

    while (1) {
        print_menu();
        printf(">> ");

        int choice;
        scanf("%d", &choice);

        if (choice == 3) break;

        int sock = 0;
        struct sockaddr_in serv_addr;

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\nSocket creation error\n");
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            printf("\nInvalid address\n");
            return -1;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            printf("\nConnection Failed\n");
            return -1;
        }

        if (choice == 1) {
            char filename[256];
            printf("Enter the input filename (e.g., input_1.txt): ");
            scanf("%255s", filename);

            // Kirim hanya nama file ke server
            char command[MAX_BUFFER];
            snprintf(command, sizeof(command), "DECRYPT %s", filename);
            send(sock, command, strlen(command), 0);

            // Terima respons
            char response[MAX_BUFFER] = {0};
            read(sock, response, MAX_BUFFER);
            printf("\nServer: Text decrypted and saved as %s\n", response);
        }
        else if (choice == 2) {
            char filename[256];
            printf("Enter the file name: ");
            scanf("%255s", filename);

            char command[MAX_BUFFER];
            snprintf(command, sizeof(command), "DOWNLOAD %s", filename);
            send(sock, command, strlen(command), 0);

            unsigned char buffer[MAX_BUFFER] = {0};
            read(sock, buffer, MAX_BUFFER);

            if (strncmp((char *)buffer, "ERROR", 5) == 0) {
                printf("\n%s\n", (char *)buffer);
            } else {
                char client_path[300];
                snprintf(client_path, sizeof(client_path), "client/%s", filename);
                FILE *file = fopen(client_path, "wb");
                fwrite(buffer, 1, MAX_BUFFER, file);
                fclose(file);
                printf("\nSuccess! Image saved as %s\n", client_path);
            }
        }

        close(sock);
    }

    return 0;
}
