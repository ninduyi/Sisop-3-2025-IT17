#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8080
#define MAX_BUFFER 4096
#define DATABASE_DIR "server/database/"
#define LOG_FILE "server/server.log"
#define INPUT_DIR "client/secrets/"

void hex_to_bin(const char *hex, unsigned char *bin, size_t len) {
    for (size_t i = 0; i < len; i += 2)
        sscanf(hex + i, "%2hhx", &bin[i / 2]);
}

void log_action(const char *source, const char *action, const char *info) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

    mkdir("server", 0777);
    FILE *log = fopen(LOG_FILE, "a");
    if (log) {
        fprintf(log, "[%s][%s]: [%s] [%s]\n", source, timestamp, action, info);
        fclose(log);
    }
}

void handle_client(int client_sock) {
    char buffer[MAX_BUFFER];

    while (1) {
        memset(buffer, 0, MAX_BUFFER);
        int read_len = read(client_sock, buffer, MAX_BUFFER);
        if (read_len <= 0) break;

        if (strncmp(buffer, "DECRYPT", 7) == 0) {
            char *filename = buffer + 8;
            log_action("Client", "DECRYPT", filename);

            char path[300];
            snprintf(path, sizeof(path), "%s%s", INPUT_DIR, filename);

            FILE *file = fopen(path, "r");
            if (!file) {
                send(client_sock, "ERROR", 5, 0);
                log_action("Server", "ERROR", "File not found");
                continue;
            }

            fseek(file, 0, SEEK_END);
            long size = ftell(file);
            fseek(file, 0, SEEK_SET);

            char *content = malloc(size + 1);
            fread(content, 1, size, file);
            fclose(file);

            for (int i = 0; i < size / 2; i++) {
                char temp = content[i];
                content[i] = content[size - i - 1];
                content[size - i - 1] = temp;
            }

            unsigned char *bin = malloc(size / 2);
            hex_to_bin(content, bin, size);

            time_t timestamp = time(NULL);
            char jpeg_name[64];
            snprintf(jpeg_name, sizeof(jpeg_name), "%ld.jpeg", timestamp);

            mkdir(DATABASE_DIR, 0777);
            char jpeg_path[300];
            snprintf(jpeg_path, sizeof(jpeg_path), "%s%s", DATABASE_DIR, jpeg_name);
            FILE *jpeg = fopen(jpeg_path, "wb");
            fwrite(bin, 1, size / 2, jpeg);
            fclose(jpeg);

            send(client_sock, jpeg_name, strlen(jpeg_name), 0);
            log_action("Server", "SAVE", jpeg_name);

            free(content);
            free(bin);
        }

        else if (strncmp(buffer, "DOWNLOAD", 8) == 0) {
            char *filename = buffer + 9;
            log_action("Client", "DOWNLOAD", filename);

            char path[300];
            snprintf(path, sizeof(path), "%s%s", DATABASE_DIR, filename);

            FILE *file = fopen(path, "rb");
            if (!file) {
                send(client_sock, "ERROR", 5, 0);
                continue;
            }

            fseek(file, 0, SEEK_END);
            int size = ftell(file);
            rewind(file);

            unsigned char *data = malloc(size);
            fread(data, 1, size, file);
            fclose(file);

            send(client_sock, &size, sizeof(int), 0);
            send(client_sock, data, size, 0);
            log_action("Server", "UPLOAD", filename);
            free(data);
        }
    }

    close(client_sock);
}

int main() {
    pid_t pid = fork();
    if (pid > 0) exit(EXIT_SUCCESS);
    umask(0);
    setsid();

    mkdir(DATABASE_DIR, 0777);

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen))) {
            handle_client(new_socket);
        }
    }

    return 0;
}
