#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
int shmid_hunter, shmid_dungeon;

const char *dungeon_names[] = {
    "Double Dungeon", "Demon Castle", "Pyramid Dungeon", "Red Gate Dungeon",
    "Hunters Guild Dungeon", "Busan A-Rank Dungeon", "Insects Dungeon",
    "Goblins Dungeon", "D-Rank Dungeon", "Gwanak Mountain Dungeon",
    "Hapjeong Subway Station Dungeon"
};

void attach_shared_memory() {
    shmid_hunter = shmget(SHM_KEY_HUNTER, sizeof(Hunter) * MAX_HUNTERS, IPC_CREAT | 0666);
    shmid_dungeon = shmget(SHM_KEY_DUNGEON, sizeof(Dungeon) * MAX_DUNGEONS, IPC_CREAT | 0666);
    hunters = (Hunter *)shmat(shmid_hunter, NULL, 0);
    dungeons = (Dungeon *)shmat(shmid_dungeon, NULL, 0);
}

void detach_and_cleanup() {
    shmdt(hunters);
    shmdt(dungeons);
    shmctl(shmid_hunter, IPC_RMID, NULL);
    shmctl(shmid_dungeon, IPC_RMID, NULL);
    printf("\n[INFO] Sistem dimatikan. Shared memory telah dihapus.\n");
}

void show_hunters() {
    printf("\n=== HUNTER INFO ===\n");
    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (hunters[i].in_use) {
            printf("Name: %s | Level: %d | EXP: %d | ATK: %d | HP: %d | DEF: %d | Status: %s\n",
                   hunters[i].name, hunters[i].level, hunters[i].exp,
                   hunters[i].atk, hunters[i].hp, hunters[i].def,
                   hunters[i].banned ? "BANNED" : "ACTIVE");
        }
    }
}

void show_dungeons() {
    printf("\n=== DUNGEON INFO ===\n");
    for (int i = 0; i < MAX_DUNGEONS; i++) {
        if (dungeons[i].in_use) {
            printf("Name: %s | Min Level: %d | EXP: %d | ATK: %d | HP: %d | DEF: %d | Key: %ld\n",
                   dungeons[i].name, dungeons[i].min_level, dungeons[i].exp_reward,
                   dungeons[i].atk_reward, dungeons[i].hp_reward, dungeons[i].def_reward,
                   dungeons[i].key);
        }
    }
}

void generate_dungeon() {
    for (int i = 0; i < MAX_DUNGEONS; i++) {
        if (!dungeons[i].in_use) {
            Dungeon *d = &dungeons[i];
            d->in_use = 1;
            strncpy(d->name, dungeon_names[rand() % 11], NAME_LEN);
            d->min_level = rand() % 5 + 1;
            d->atk_reward = rand() % 51 + 100;
            d->hp_reward = rand() % 51 + 50;
            d->def_reward = rand() % 26 + 25;
            d->exp_reward = rand() % 151 + 150;
            d->key = time(NULL) + rand();
            printf("Dungeon generated!\n");
            return;
        }
    }
    printf("Dungeon memory full.\n");
}

void ban_hunter() {
    printf("=== LIST HUNTER ===\n");
    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (hunters[i].in_use)
            printf("- %s (%s)\n", hunters[i].name, hunters[i].banned ? "BANNED" : "ACTIVE");
    }

    char name[NAME_LEN];
    printf("Hunter name to (un)ban: ");
    fgets(name, NAME_LEN, stdin);
    name[strcspn(name, "\n")] = 0;

    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (hunters[i].in_use && strcmp(hunters[i].name, name) == 0) {
            hunters[i].banned = !hunters[i].banned;
            printf("Hunter %s is now %s\n", name, hunters[i].banned ? "BANNED" : "UNBANNED");
            return;
        }
    }
    printf("Hunter not found.\n");
}

void reset_hunter() {
    char name[NAME_LEN];
    printf("Hunter name to reset: ");
    fgets(name, NAME_LEN, stdin);
    name[strcspn(name, "\n")] = 0;
    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (hunters[i].in_use && strcmp(hunters[i].name, name) == 0) {
            hunters[i].level = 1; hunters[i].exp = 0; hunters[i].atk = 10;
            hunters[i].hp = 100; hunters[i].def = 5; hunters[i].banned = 0;
            printf("Stats reset.\n");
            return;
        }
    }
    printf("Hunter not found.\n");
}

void menu() {
    int c;
    while (1) {
        printf("\n=== SYSTEM MENU ===\n1. Hunter Info\n2. Dungeon Info\n3. Generate Dungeon\n4. Ban Hunter\n5. Reset Hunter\n6. Exit\nChoice: ");
        if (scanf("%d", &c) != 1) {
            printf("Input tidak valid! Masukkan angka.\n");
            while (getchar() != '\n');
            continue;
        }
        getchar();
        if (c == 1) show_hunters();
        else if (c == 2) show_dungeons();
        else if (c == 3) generate_dungeon();
        else if (c == 4) ban_hunter();
        else if (c == 5) reset_hunter();
        else if (c == 6) break;
        else printf("Pilihan tidak tersedia. Masukkan angka 1-6.\n");
    }
    detach_and_cleanup();
}

int main() {
    srand(time(NULL));
    attach_shared_memory();
    menu();
    return 0;
}
