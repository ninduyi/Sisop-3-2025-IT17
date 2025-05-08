#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define MAX_HUNTERS 100
#define MAX_DUNGEONS 100
#define NAME_LEN 32

typedef struct {
    char name[NAME_LEN];
    int level, exp, atk, hp, def;
    int banned;
    int in_use;
    long key;  // key untuk shared memory hunter (key unik per hunter)
} Hunter;

typedef struct {
    char name[NAME_LEN];
    int min_level, exp_reward, atk_reward, hp_reward, def_reward;
    long key;  // key untuk shared memory dungeon (key unik per dungeon)
    int in_use;
} Dungeon;

#define SHM_KEY_HUNTER 0x1234
#define SHM_KEY_DUNGEON 0x5678

Hunter *hunters;
Dungeon *dungeons;
Hunter *me = NULL;

int notif_on = 0;
int stop_notif = 0;
int notif_paused = 0;
int notif_thread_running = 0;
pthread_t notif_thread;
int notif_index = 0;

int shmid_hunter, shmid_dungeon;  // Global shared memory IDs

const char *dungeon_names[] = {
    "Double Dungeon", "Demon Castle", "Pyramid Dungeon", "Red Gate Dungeon",
    "Hunters Guild Dungeon", "Busan A-Rank Dungeon", "Insects Dungeon",
    "Goblins Dungeon", "D-Rank Dungeon", "Gwanak Mountain Dungeon",
    "Demon King", "Dragon's Lair", "Underground Dungeon"
};

void press_enter() {
    printf("\nPress enter to continue...");
    getchar();
    system("clear");
}

void *notification_loop(void *arg) {
    while (!stop_notif) {
        if (notif_on && !notif_paused && me != NULL) {
            system("clear");
            printf("======== HUNTER SYSTEM ========\n");

            int count = 0, showIndex = -1;
            for (int i = 0; i < MAX_DUNGEONS; i++) {
                if (dungeons[i].in_use) {
                    if (count == notif_index) showIndex = i;
                    count++;
                }
            }
            if (count > 0 && showIndex != -1) {
                Dungeon *d = &dungeons[showIndex];
                printf("Dungeon: %s (Min Level: %d)\n", d->name, d->min_level);
                notif_index = (notif_index + 1) % count;
            } else {
                printf("No available dungeons.\n");
            }

            printf("\n======== %s's MENU ========\n", me->name);
            printf("1. Dungeon List\n2. Dungeon Raid\n3. Hunters Battle\n4. Notification\n5. Exit\n");
            printf("Choice: ");
            fflush(stdout);
            sleep(3);
        } else {
            usleep(200000);
        }
    }
    return NULL;
}

void attach_shm() {
    shmid_hunter = shmget(SHM_KEY_HUNTER, sizeof(Hunter) * MAX_HUNTERS, 0666);
    shmid_dungeon = shmget(SHM_KEY_DUNGEON, sizeof(Dungeon) * MAX_DUNGEONS, 0666);
    if (shmid_hunter == -1 || shmid_dungeon == -1) {
        printf("[ERROR] Sistem belum dijalankan. Jalankan system.c terlebih dahulu.\n");
        exit(1);
    }
    hunters = shmat(shmid_hunter, NULL, 0);
    dungeons = shmat(shmid_dungeon, NULL, 0);
}

void detach_and_cleanup() {
    // Detach shared memory hunters and dungeons
    shmdt(hunters);
    shmdt(dungeons);

    // Menghapus shared memory dungeon dan hunter setelah selesai
    if (shmctl(shmid_hunter, IPC_RMID, NULL) == -1) {
        perror("Failed to remove shared memory for hunters");
    } else {
        printf("Hunter shared memory removed.\n");
    }

    if (shmctl(shmid_dungeon, IPC_RMID, NULL) == -1) {
        perror("Failed to remove shared memory for dungeons");
    } else {
        printf("Dungeon shared memory removed.\n");
    }

    printf("All shared memory cleaned up and system is shutting down.\n");
}

void register_hunter() {
    char name[NAME_LEN];
    printf("Username: "); fgets(name, NAME_LEN, stdin); name[strcspn(name, "\n")] = 0;

    // Generate unique key for the hunter
    long key = time(NULL) + rand();  // Create a unique key based on time and random value

    // Membuat shared memory baru untuk hunter
    int shm_id = shmget(key, sizeof(Hunter), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Failed to create shared memory for hunter");
        return;
    }

    Hunter *new_hunter = shmat(shm_id, NULL, 0);
    if (new_hunter == (void *) -1) {
        perror("Failed to attach shared memory for hunter");
        return;
    }

    // Mengisi data hunter baru
    strcpy(new_hunter->name, name);
    new_hunter->level = 1;
    new_hunter->exp = 0;
    new_hunter->atk = 10;
    new_hunter->hp = 100;
    new_hunter->def = 5;
    new_hunter->banned = 0;
    new_hunter->in_use = 1;
    new_hunter->key = key;  // Menyimpan key untuk shared memory

    // Menambahkan hunter ke shared memory utama
    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (!hunters[i].in_use) {
            hunters[i] = *new_hunter;
            printf("Hunter registration success!\n");
            press_enter();
            return;
        }
    }

    printf("Hunter registration failed.\n");
    press_enter();
    shmdt(new_hunter); // Melepaskan shared memory setelah selesai
}

int login_hunter() {
    char name[NAME_LEN];
    printf("Username: "); fgets(name, NAME_LEN, stdin); name[strcspn(name, "\n")] = 0;

    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (hunters[i].in_use && strcmp(hunters[i].name, name) == 0) {
            me = &hunters[i];
            system("clear");
            return 1;
        }
    }
    printf("Login failed.\n");
    press_enter();
    return 0;
}

void show_dungeons() {
    notif_paused = 1;
    system("clear");
    printf("=== AVAILABLE DUNGEONS ===\n");
    int count = 0;
    for (int i = 0; i < MAX_DUNGEONS; i++) {
        if (dungeons[i].in_use && dungeons[i].min_level <= me->level) {
            printf("%d. %s (Level %d+)\n", ++count, dungeons[i].name, dungeons[i].min_level);
        }
    }
    if (count == 0) printf("No available dungeons.\n");
    press_enter();
    notif_paused = 0;
}

void raid_dungeon() {
    notif_paused = 1;
    if (me->banned) {
        printf("You are BANNED. You cannot raid.\n");
        press_enter();
        notif_paused = 0;
        return;
    }

    system("clear");
    printf("=== RAIDABLE DUNGEONS ===\n");
    int idx_map[MAX_DUNGEONS], idx = 0;
    for (int i = 0; i < MAX_DUNGEONS; i++) {
        if (dungeons[i].in_use && dungeons[i].min_level <= me->level) {
            printf("%d. %s (Level %d+)\n", idx + 1, dungeons[i].name, dungeons[i].min_level);
            idx_map[idx++] = i;
        }
    }
    if (idx == 0) {
        printf("No dungeon available.\n");
        press_enter();
        notif_paused = 0;
        return;
    }

    int ch;
    printf("Choose Dungeon: ");
    scanf("%d", &ch); getchar();
    if (ch < 1 || ch > idx) {
        printf("Invalid choice.\n");
        press_enter();
        notif_paused = 0;
        return;
    }

    // Akses shared memory dungeon yang dipilih
    Dungeon *d = &dungeons[idx_map[ch - 1]];

    // Raid sukses dan mendapatkan rewards
    printf("\nRaid success! Gained:\n");
    printf("ATK: %d\nHP: %d\nDEF: %d\nEXP: %d\n", d->atk_reward, d->hp_reward, d->def_reward, d->exp_reward);

    me->atk += d->atk_reward;
    me->hp += d->hp_reward;
    me->def += d->def_reward;
    me->exp += d->exp_reward;
    if (me->exp >= 500) { me->level++; me->exp = 0; }

    // Dungeon yang telah selesai di-raid tidak bisa digunakan lagi
    d->in_use = 0;

    // Menghapus shared memory dungeon setelah selesai
    int shm_id = shmget(d->key, sizeof(Dungeon), 0666);
    if (shm_id != -1) {
        printf("Shmid for dungeon %ld: %d\n", d->key, shm_id);
        
        // Detach shared memory dungeon
        if (shmdt(d) == -1) {
            perror("Failed to detach shared memory for dungeon");
        } else {
            printf("Dungeon shared memory detached.\n");
        }

        // Hapus shared memory
        if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
            perror("Failed to remove shared memory for dungeon");
        } else {
            printf("Dungeon's shared memory removed successfully.\n");
        }
    }

    press_enter();
    notif_paused = 0;
}

void battle() {
    notif_paused = 1;
    if (me->banned) {
        printf("You are BANNED. You cannot battle.\n");
        press_enter();
        notif_paused = 0;
        return;
    }

    system("clear");
    printf("======= Choose Who's to Fight =======\n");
    int found = 0;
    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (&hunters[i] != me && hunters[i].in_use) {
            printf("- %s (Power: %d)\n", hunters[i].name, hunters[i].atk + hunters[i].hp + hunters[i].def);
            found = 1;
        }
    }
    if (!found) {
        printf("No available opponents.\n");
        press_enter();
        notif_paused = 0;
        return;
    }

    char name[NAME_LEN];
    printf("Input name: "); fgets(name, NAME_LEN, stdin); name[strcspn(name, "\n")] = 0;

    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (hunters[i].in_use && strcmp(hunters[i].name, name) == 0 && &hunters[i] != me) {
            Hunter *op = &hunters[i];
            printf("\n=== PVP LIST ===\n");
            printf("%s - Total Power: %d\n", op->name, op->atk + op->hp + op->def);
            printf("Target: %s\nYou chose to battle %s\n", op->name, op->name);
            printf("Your Power: %d\nOpponent's Power: %d\n", me->atk + me->hp + me->def, op->atk + op->hp + op->def);

            if ((me->atk + me->hp + me->def) >= (op->atk + op->hp + op->def)) {
                me->atk += op->atk;
                me->hp += op->hp;
                me->def += op->def;
                op->in_use = 0;

                // Menghapus shared memory untuk hunter yang kalah
                int shm_id = shmget(op->key, sizeof(Hunter), 0666);
                if (shm_id != -1) {
                    shmctl(shm_id, IPC_RMID, NULL);
                    printf("Opponent's shared memory removed.\n");
                }

                printf("Battle won! You acquired %s's stats\n", op->name);
            } else {
                op->atk += me->atk;
                op->hp += me->hp;
                op->def += me->def;
                me->in_use = 0;

                // Menghapus shared memory untuk hunter yang kalah
                int shm_id = shmget(me->key, sizeof(Hunter), 0666);
                if (shm_id != -1) {
                    shmctl(shm_id, IPC_RMID, NULL);
                    printf("Your shared memory removed.\n");
                }

                printf("You lost. Eliminated.\n");
            }
            press_enter();
            notif_paused = 0;
            return;
        }
    }
    printf("Target not found.\n");
    press_enter();
    notif_paused = 0;
}

void hunter_system() {
    notif_index = 0;
    stop_notif = 0;

    int ch;
    while (me->in_use) {
        if (!notif_on) {
            printf("======== HUNTER SYSTEM ========\n\n");
            printf("======== %s's MENU ========\n", me->name);
            printf("1. Dungeon List\n2. Dungeon Raid\n3. Hunters Battle\n4. Notification\n5. Exit\n");
            printf("Choice: ");
        }

        scanf("%d", &ch); getchar();

        if (ch == 1) show_dungeons();
        else if (ch == 2) raid_dungeon();
        else if (ch == 3) battle();
        else if (ch == 4) {
            char yn;
            printf("Aktifkan notifikasi? (y/n): ");
            scanf(" %c", &yn); getchar();
            notif_on = (yn == 'y' || yn == 'Y');
            printf("Notification turned %s.\n", notif_on ? "ON" : "OFF");
            press_enter();

            if (notif_on && !notif_thread_running) {
                pthread_create(&notif_thread, NULL, notification_loop, NULL);
                notif_thread_running = 1;
            }
        }
        else if (ch == 5) break;
        else { printf("Invalid choice.\n"); press_enter(); }
    }

    stop_notif = 1;
    if (notif_thread_running) {
        pthread_join(notif_thread, NULL);
        notif_thread_running = 0;
    }

    system("clear"); // pindah halaman ke HUNTER MENU
}

int main() {
    attach_shm();
    int c;
    while (1) {
        notif_on = 0;
        stop_notif = 1;
        notif_paused = 0;
        notif_thread_running = 0;

        printf("======== HUNTER MENU ========\n");
        printf("1. Register\n2. Login\n3. Exit\nChoice: ");
        scanf("%d", &c); getchar();
        system("clear");
        if (c == 1) register_hunter();
        else if (c == 2 && login_hunter()) hunter_system();
        else if (c == 3) break;
    }
    return 0;
}
