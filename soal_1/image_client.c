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
    remove(zip_filename);
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
        download_file(ZIP_URL, TEMP_ZIP);
        extract_zip(TEMP_ZIP, "client");
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Failed to connect to server.\n");
        return -1;
    }

    printf("Connected to address 127.0.0.1:%d\n", PORT);

    while (1) {
        print_menu();
        printf(">> ");
        char input[16];
        fgets(input, sizeof(input), stdin);
        int choice = atoi(input);

        if (choice == 3) {
            char exit_msg[MAX_BUFFER] = "Client requested to exit";
            send(sock, exit_msg, strlen(exit_msg), 0);
            break;
        }

        if (choice == 1) {
            char filename[256];
            printf("Enter the input filename (e.g., input_1.txt): ");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = 0;

            char command[MAX_BUFFER];
            snprintf(command, sizeof(command), "DECRYPT %s", filename);
            send(sock, command, strlen(command), 0);

            char response[MAX_BUFFER] = {0};
            read(sock, response, MAX_BUFFER);

            if (strcmp(response, "ERROR: File not found") == 0) {
                printf("File not found.\n");
            } else {
                printf("\nServer: Text decrypted and saved as %s\n", response);
            }
        }

        else if (choice == 2) {
            char filename[256];
            printf("Enter the file name: ");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = 0;

            char command[MAX_BUFFER];
            snprintf(command, sizeof(command), "DOWNLOAD %s", filename);
            send(sock, command, strlen(command), 0);

            int filesize = 0;
            int r = recv(sock, &filesize, sizeof(int), MSG_WAITALL);

            if (r != sizeof(int) || filesize <= 0 || filesize > 10000000) {
                printf("File not found.\n");
                continue;
            }

            unsigned char *buffer = malloc(filesize);
            int received = 0;
            while (received < filesize) {
                int n = recv(sock, buffer + received, filesize - received, 0);
                if (n <= 0) break;
                received += n;
            }

            if (received != filesize) {
                free(buffer);
                continue;
            }

            char path[300];
            snprintf(path, sizeof(path), "client/%s", filename);
            FILE *out = fopen(path, "wb");
            fwrite(buffer, 1, filesize, out);
            fclose(out);
            free(buffer);

            printf("Success! Image saved as %s\n", filename);
        }

        else {
            printf("Invalid option.\n");
        }
    }

    close(sock);
    return 0;
}
