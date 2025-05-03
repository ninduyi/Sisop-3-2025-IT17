#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
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

Order *orders;

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

void *agent_thread(void *arg) {
    char *agent = (char *)arg;
    while (1) {
        for (int i = 0; i < MAX_ORDER; i++) {
            if (strcmp(orders[i].type, "Express") == 0 && strcmp(orders[i].status, "Pending") == 0) {
                strcpy(orders[i].status, "Delivered");
                strcpy(orders[i].agent, agent);
                log_delivery(agent, "Express", orders[i].name, orders[i].address);
                sleep(1);
            }
        }
        sleep(2);
    }
    return NULL;
}

int main() {
    key_t key = ftok("rushgo", 65);
    int shmid = shmget(key, sizeof(Order) * MAX_ORDER, 0666);
    orders = (Order *) shmat(shmid, NULL, 0);

    pthread_t a, b, c;
    pthread_create(&a, NULL, agent_thread, "A");
    pthread_create(&b, NULL, agent_thread, "B");
    pthread_create(&c, NULL, agent_thread, "C");

    pthread_join(a, NULL);
    pthread_join(b, NULL);
    pthread_join(c, NULL);

    shmdt(orders);
    return 0;
}
