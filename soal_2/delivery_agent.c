#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define MAX_ORDERS 100
#define NAME_LEN 50
#define ADDR_LEN 100
#define AGENT_COUNT 3

const char* agent_names[AGENT_COUNT] = {"AGENT A", "AGENT B", "AGENT C"};

typedef struct {
    char nama[NAME_LEN];
    char alamat[ADDR_LEN];
    char jenis[10];
    int delivered; 
    char delivered_by[NAME_LEN];
} Order;

Order *orders;
int *order_count;
pthread_mutex_t *mutex;

void log_delivery(const char* agent, const Order* order) {
    FILE *log = fopen("delivery.log", "a");
    if (!log) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log, "[%02d/%02d/%04d %02d:%02d:%02d] [%s] Express package delivered to %s in %s\n",
        t->tm_mday, t->tm_mon+1, t->tm_year+1900,
        t->tm_hour, t->tm_min, t->tm_sec,
        agent, order->nama, order->alamat);
    fclose(log);
}

typedef struct {
    int agent_id;
    const char* agent_name;
} AgentArg;

void* agent_thread(void* arg) {
    AgentArg* agent = (AgentArg*)arg;
    while (1) {
        for (int i = 0; i < *order_count; i++) {
            pthread_mutex_lock(mutex);
            if (strcmp(orders[i].jenis, "Express") == 0 && !orders[i].delivered && (i % AGENT_COUNT == agent->agent_id)) {
                orders[i].delivered = 1;
                strncpy(orders[i].delivered_by, agent->agent_name, NAME_LEN);
                log_delivery(agent->agent_name, &orders[i]);
            }
            pthread_mutex_unlock(mutex);
        }
        sleep(1);
    }
    return NULL;
}

int main() {
    int shm_fd = shm_open("/orders", O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(Order) * MAX_ORDERS + sizeof(int) + sizeof(pthread_mutex_t));
    void* ptr = mmap(0, sizeof(Order) * MAX_ORDERS + sizeof(int) + sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    orders = (Order*)ptr;
    order_count = (int*)(ptr + sizeof(Order) * MAX_ORDERS);
    mutex = (pthread_mutex_t*)(ptr + sizeof(Order) * MAX_ORDERS + sizeof(int));

    pthread_t threads[AGENT_COUNT];
    AgentArg args[AGENT_COUNT];
    for (int i = 0; i < AGENT_COUNT; i++) {
        args[i].agent_id = i;
        args[i].agent_name = agent_names[i];
        pthread_create(&threads[i], NULL, agent_thread, &args[i]);
    }
    for (int i = 0; i < AGENT_COUNT; i++) pthread_join(threads[i], NULL);
    return 0;
}
