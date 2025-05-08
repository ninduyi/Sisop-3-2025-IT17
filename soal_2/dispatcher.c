
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <pwd.h>

#define MAX_ORDERS 100
#define NAME_LEN 50
#define ADDR_LEN 100

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

void log_delivery(const char* agent, const Order* order, const char* tipe) {
    FILE *log = fopen("delivery.log", "a");
    if (!log) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log, "[%02d/%02d/%04d %02d:%02d:%02d] [%s] %s package delivered to %s in %s\n",
        t->tm_mday, t->tm_mon+1, t->tm_year+1900,
        t->tm_hour, t->tm_min, t->tm_sec,
        agent, tipe, order->nama, order->alamat);
    fclose(log);
}

void list_orders() {
    for (int i = 0; i < *order_count; i++) {
        printf("%s - %s\n", orders[i].nama, orders[i].delivered ? orders[i].delivered_by : "Pending");
    }
}

void check_status(const char* name) {
    for (int i = 0; i < *order_count; i++) {
        if (strcmp(orders[i].nama, name) == 0) {
            if (orders[i].delivered)
                printf("Status for %s: Delivered by %s\n", name, orders[i].delivered_by);
            else
                printf("Status for %s: Pending\n", name);
            return;
        }
    }
    printf("No order found for %s\n", name);
}

void deliver_manual(const char* name, const char* user) {
    for (int i = 0; i < *order_count; i++) {
        if (strcmp(orders[i].nama, name) == 0 && strcmp(orders[i].jenis, "Reguler") == 0 && !orders[i].delivered) {
            pthread_mutex_lock(mutex);
            orders[i].delivered = 1;
            strncpy(orders[i].delivered_by, user, NAME_LEN);
            log_delivery(user, &orders[i], "Reguler");
            pthread_mutex_unlock(mutex);
            printf("Delivered Reguler order for %s by %s\n", name, user);
            return;
        }
    }
    printf("No pending Reguler order found for %s\n", name);
}

void load_csv_to_shared_memory(const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open CSV");
        exit(1);
    }
    char line[200];
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%49[^,],%99[^,],%9s", orders[*order_count].nama, orders[*order_count].alamat, orders[*order_count].jenis) == 3) {
            orders[*order_count].delivered = 0;
            orders[*order_count].delivered_by[0] = '\0';
            (*order_count)++;
        }
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    int shm_fd = shm_open("/orders", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(Order) * MAX_ORDERS + sizeof(int) + sizeof(pthread_mutex_t));
    void* ptr = mmap(0, sizeof(Order) * MAX_ORDERS + sizeof(int) + sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    orders = (Order*)ptr;
    order_count = (int*)(ptr + sizeof(Order) * MAX_ORDERS);
    mutex = (pthread_mutex_t*)(ptr + sizeof(Order) * MAX_ORDERS + sizeof(int));

    if (argc >= 2 && strcmp(argv[1], "-deliver") == 0 && argc == 3) {
        struct passwd *pw = getpwuid(getuid());
        deliver_manual(argv[2], pw->pw_name);
    } else if (argc >= 2 && strcmp(argv[1], "-status") == 0 && argc == 3) {
        check_status(argv[2]);
    } else if (argc >= 2 && strcmp(argv[1], "-list") == 0) {
        list_orders();
    } else if (argc >= 2 && strcmp(argv[1], "-init") == 0) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(mutex, &attr);
        *order_count = 0;
        load_csv_to_shared_memory("delivery_order.csv");
        printf("Shared memory initialized with orders.\n");
    } else {
        printf("Usage:\n  ./dispatcher -init\n  ./dispatcher -deliver [Nama]\n  ./dispatcher -status [Nama]\n  ./dispatcher -list\n");
    }

    return 0;
}

