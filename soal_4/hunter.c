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
} Hunter;

typedef struct {
    char name[NAME_LEN];
    int min_level, exp_reward, atk_reward, hp_reward, def_reward;
    long key;
    int in_use;
} Dungeon;

#define SHM_KEY_HUNTER 0x1234
#define SHM_KEY_DUNGEON 0x5678

Hunter *hunters;
Dungeon *dungeons;
Hunter *me = NULL;

pthread_t notif_thread;
int notif_active = 0;
int notif_paused = 0;
pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

void* dungeon_notifier(void* arg) {
    static int idx = 0;
    while (1) {
        if (notif_paused) { sleep(1); continue; }

        pthread_mutex_lock(&print_lock);
        printf("\033[2;1H\033[K");
        int count = 0, targetIndex = -1;
        for (int i = 0; i < MAX_DUNGEONS; i++) {
            if (dungeons[i].in_use) {
                if (count == idx) targetIndex = i;
                count++;
            }
        }

        if (count == 0) {
            printf("No dungeons available.");
        } else if (targetIndex != -1) {
            Dungeon *d = &dungeons[targetIndex];
            printf("Dungeon: %s (Min Level: %d)", d->name, d->min_level);
            idx = (idx + 1) % count;
        }

        printf("\033[10;9H");
        fflush(stdout);
        pthread_mutex_unlock(&print_lock);
        sleep(3);
    }
    return NULL;
}

void press_enter() {
    printf("\nPress ENTER to continue...");
    getchar();
    system("clear");
}

void attach_shm() {
    int shm_id_h = shmget(SHM_KEY_HUNTER, sizeof(Hunter)*MAX_HUNTERS, 0666);
    int shm_id_d = shmget(SHM_KEY_DUNGEON, sizeof(Dungeon)*MAX_DUNGEONS, 0666);
    if (shm_id_h == -1 || shm_id_d == -1) {
        printf("[ERROR] Sistem belum dijalankan. Jalankan system.c terlebih dahulu.\n");
        exit(1);
    }
    hunters = shmat(shm_id_h, NULL, 0);
    dungeons = shmat(shm_id_d, NULL, 0);
}

void register_hunter() {
    char name[NAME_LEN];
    printf("Username: "); fgets(name, NAME_LEN, stdin); name[strcspn(name, "\n")] = 0;
    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (!hunters[i].in_use) {
            strcpy(hunters[i].name, name);
            hunters[i].level = 1; hunters[i].exp = 0; hunters[i].atk = 10;
            hunters[i].hp = 100; hunters[i].def = 5; hunters[i].banned = 0;
            hunters[i].in_use = 1;
            printf("Registration success!\n"); press_enter(); return;
        }
    }
    printf("Registration failed.\n"); press_enter();
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
    printf("Login failed.\n"); press_enter(); return 0;
}

void show_dungeons() {
    printf("=== AVAILABLE DUNGEONS ===\n");
    for (int i = 0; i < MAX_DUNGEONS; i++)
        if (dungeons[i].in_use && dungeons[i].min_level <= me->level)
            printf("%d. %s (Level %d+)\n", i + 1, dungeons[i].name, dungeons[i].min_level);
    press_enter();
}

void raid_dungeon() {
    notif_paused = 1;
    if (notif_active) system("clear");

    int ch;
    printf("=== RAIDABLE DUNGEONS ===\n");
    for (int i = 0; i < MAX_DUNGEONS; i++)
        if (dungeons[i].in_use && dungeons[i].min_level <= me->level)
            printf("%d. %s\n", i + 1, dungeons[i].name);
    printf("Choose: "); scanf("%d", &ch); getchar(); ch--;
    if (dungeons[ch].in_use && dungeons[ch].min_level <= me->level) {
        me->atk += dungeons[ch].atk_reward;
        me->hp += dungeons[ch].hp_reward;
        me->def += dungeons[ch].def_reward;
        me->exp += dungeons[ch].exp_reward;
        if (me->exp >= 500) { me->level++; me->exp = 0; }
        dungeons[ch].in_use = 0;
        printf("Raid success!\n");
    } else printf("Invalid.\n");
    press_enter();
    notif_paused = 0;
}

void battle() {
    notif_paused = 1;
    if (notif_active) system("clear");

    printf("=== PVP LIST ===\n");
    int found = 0;
    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (&hunters[i] != me && hunters[i].in_use) {
            printf("%d. %s - Total Power: %d\n", i, hunters[i].name,
                   hunters[i].atk + hunters[i].hp + hunters[i].def);
            found = 1;
        }
    }

    if (!found) {
        printf("No available hunters to battle.\n");
        press_enter();
        notif_paused = 0;
        return;
    }

    printf("Target index: "); int idx; scanf("%d", &idx); getchar();
    if (!hunters[idx].in_use || &hunters[idx] == me) return;

    printf("You chose to battle %s\n", hunters[idx].name);
    printf("Your Power: %d\n", me->atk + me->hp + me->def);
    printf("Opponent's Power: %d\n", hunters[idx].atk + hunters[idx].hp + hunters[idx].def);

    if ((me->atk + me->hp + me->def) >= (hunters[idx].atk + hunters[idx].hp + hunters[idx].def)) {
        me->atk += hunters[idx].atk;
        me->hp += hunters[idx].hp;
        me->def += hunters[idx].def;
        hunters[idx].in_use = 0;
        printf("Battle won! You acquired %s's stats.\n", hunters[idx].name);
    } else {
        hunters[idx].atk += me->atk;
        hunters[idx].hp += me->hp;
        hunters[idx].def += me->def;
        me->in_use = 0;
        printf("You lost. Eliminated.\n");
    }

    press_enter();
    notif_paused = 0;

    if (!me->in_use && notif_active) {
        pthread_cancel(notif_thread);
        notif_active = 0;
    }
}

void hunter_system() {
    int ch;
    while (me->in_use) {
        pthread_mutex_lock(&print_lock);
        printf("\033[H\033[J");
        printf("\033[1;1H======HUNTER MENU======");
        printf("\033[4;1H======%s's MENU======\n", me->name);
        printf("1. Dungeon List\n2. Dungeon Raid\n3. Hunters Battle\n4. Notification\n5. Exit\n");
        printf("\033[10;1HChoice: ");
        pthread_mutex_unlock(&print_lock);
        scanf("%d", &ch); getchar();

        if (ch == 1) show_dungeons();
        else if (ch == 2) {
            if (me->banned) { printf("You are banned from raid.\n"); press_enter(); }
            else raid_dungeon();
        }
        else if (ch == 3) {
            if (me->banned) { printf("You are banned from battle.\n"); press_enter(); }
            else battle();
        }
        else if (ch == 4) {
            char ans;
            printf("\033[11;1H\033[KAktifkan notifikasi? (y/n): ");
            scanf(" %c", &ans); getchar();
            if (ans == 'y' || ans == 'Y') {
                if (!notif_active) {
                    notif_active = 1;
                    pthread_create(&notif_thread, NULL, dungeon_notifier, NULL);
                }
            } else {
                if (notif_active) {
                    pthread_cancel(notif_thread);
                    notif_active = 0;
                    printf("\033[2;1H\033[K");
                }
            }
        }
        else if (ch == 5) {
            if (notif_active) {
                pthread_cancel(notif_thread);
                notif_active = 0;
            }
            break;
        }
    }
}

int main() {
    attach_shm();
    int c;
    while (1) {
        printf("=== HUNTER MENU ===\n1. Register\n2. Login\n3. Exit\nChoice: ");
        scanf("%d", &c); getchar();
        system("clear");
        if (c == 1) register_hunter();
        else if (c == 2 && login_hunter()) hunter_system();
        else if (c == 3) break;
    }
    return 0;
}
