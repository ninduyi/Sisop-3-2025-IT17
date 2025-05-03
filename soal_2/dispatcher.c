#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_ORDER 100

typedef struct {
    char name[50];
    char address[100];
    char type[10]; // Express atau Reguler
    char status[20]; // Pending atau Delivered
    char agent[20];  // Nama agen pengantar
} Order;

void log_delivery(const char *agent, const char *type, const char *name, const char *address) {
    FILE *log = fopen("delivery.log", "a");
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log, "[%02d/%02d/%04d %02d:%02d:%02d] [AGENT %s] %s package delivered to %s in %s\n",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec,
            agent, type, name, address);
    fclose(log);
}

int main(int argc, char *argv[]) {
    key_t key = ftok("rushgo", 65);
    int shmid = shmget(key, sizeof(Order) * MAX_ORDER, IPC_CREAT | 0666);
    Order *orders = (Order *) shmat(shmid, NULL, 0);

    if (argc == 1) {
        // Load CSV
        FILE *fp = fopen("delivery_order.csv", "r");
        if (!fp) {
            perror("CSV not found");
            exit(1);
        }
        char line[256];
        int idx = 0;
        fgets(line, sizeof(line), fp); // skip header
        while (fgets(line, sizeof(line), fp)) {
            sscanf(line, "%[^,],%[^,],%s", orders[idx].name, orders[idx].address, orders[idx].type);
            strcpy(orders[idx].status, "Pending");
            strcpy(orders[idx].agent, "-");
            idx++;
        }
        fclose(fp);
        printf("Orders loaded to shared memory.\n");

    } else if (argc == 3 && strcmp(argv[1], "-deliver") == 0) {
        char *name = argv[2];
        for (int i = 0; i < MAX_ORDER; i++) {
            if (strcmp(orders[i].name, name) == 0 && strcmp(orders[i].type, "Reguler") == 0 && strcmp(orders[i].status, "Pending") == 0) {
                strcpy(orders[i].status, "Delivered");
                strcpy(orders[i].agent, name);
                log_delivery(name, "Reguler", orders[i].name, orders[i].address);
                printf("Delivered %s.\n", name);
                break;
            }
        }

    } else if (argc == 3 && strcmp(argv[1], "-status") == 0) {
        char *name = argv[2];
        for (int i = 0; i < MAX_ORDER; i++) {
            if (strcmp(orders[i].name, name) == 0) {
                if (strcmp(orders[i].status, "Delivered") == 0)
                    printf("Status for %s: Delivered by %s\n", name, orders[i].agent);
                else
                    printf("Status for %s: Pending\n", name);
                break;
            }
        }

    } else if (argc == 2 && strcmp(argv[1], "-list") == 0) {
        for (int i = 0; i < MAX_ORDER && strlen(orders[i].name) > 0; i++) {
            printf("%s - %s (%s) by %s\n", orders[i].name, orders[i].status, orders[i].type, orders[i].agent);
        }

    } else {
        printf("Invalid command.\n");
    }

    shmdt(orders);
    return 0;
}
