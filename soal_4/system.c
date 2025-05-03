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
    "Demon King", "Dragon's Lair", "Underground Dungeon"
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
    printf("Shared memory deleted. Exiting system.\n");
}

void show_hunters() {
    printf("\n================================ HUNTER INFO ================================\n");
    printf("| %-3s | %-20s | %-5s | %-5s | %-5s | %-5s | %-5s | %-8s |\n",
           "No", "Name", "Level", "EXP", "ATK", "HP", "DEF", "Status");
    printf("-------------------------------------------------------------------------------\n");

    int num = 1;
    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (hunters[i].in_use) {
            printf("| %-3d | %-20s | %-5d | %-5d | %-5d | %-5d | %-5d | %-8s |\n",
                   num++, hunters[i].name, hunters[i].level, hunters[i].exp,
                   hunters[i].atk, hunters[i].hp, hunters[i].def,
                   hunters[i].banned ? "BANNED" : "ACTIVE");
        }
    }
    printf("===============================================================================\n");
}

void show_dungeons() {
    printf("\n==================================== DUNGEON INFO ====================================\n");
    printf("| %-3s | %-25s | %-10s | %-5s | %-5s | %-5s | %-12s |\n",
           "No", "Name", "Min Level", "EXP", "ATK", "HP", "Key");
    printf("---------------------------------------------------------------------------------------\n");

    int num = 1;
    for (int i = 0; i < MAX_DUNGEONS; i++) {
        if (dungeons[i].in_use) {
            printf("| %-3d | %-25s | %-10d | %-5d | %-5d | %-5d | %-12ld |\n",
                   num++, dungeons[i].name, dungeons[i].min_level,
                   dungeons[i].exp_reward, dungeons[i].atk_reward,
                   dungeons[i].hp_reward, dungeons[i].key);
        }
    }
    printf("======================================================================================\n");
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
            printf("\nDungeon generated!\n");
            printf("Name           : %s\n", d->name);
            printf("Minimum Level  : %d\n", d->min_level);
            return;
        }
    }
    printf("Dungeon memory full.\n");
}

void ban_hunter() {
    int choice;
    char name[NAME_LEN];

    printf("\n=========== BAN/UNBAN MENU ===========\n");
    printf("1. Ban Hunter\n");
    printf("2. Unban Hunter\n");
    printf("Choice: ");
    scanf("%d", &choice); getchar();

    if (choice != 1 && choice != 2) {
        printf("Invalid choice.\n");
        return;
    }

    printf("\n=========== HUNTER LIST ===========\n");
    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (hunters[i].in_use) {
            printf("- %s [%s]\n", hunters[i].name, hunters[i].banned ? "BANNED" : "ACTIVE");
        }
    }

    printf("Enter hunter name: ");
    fgets(name, NAME_LEN, stdin);
    name[strcspn(name, "\n")] = 0;

    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (hunters[i].in_use && strcmp(hunters[i].name, name) == 0) {
            hunters[i].banned = (choice == 1 ? 1 : 0);
            printf("Hunter %s is now %s.\n", name, hunters[i].banned ? "BANNED" : "UNBANNED");
            return;
        }
    }

    printf("Hunter not found.\n");
}

void reset_hunter() {
    char name[NAME_LEN];
    printf("\nEnter hunter name to reset: ");
    fgets(name, NAME_LEN, stdin);
    name[strcspn(name, "\n")] = 0;
    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (hunters[i].in_use && strcmp(hunters[i].name, name) == 0) {
            hunters[i].level = 1;
            hunters[i].exp = 0;
            hunters[i].atk = 10;
            hunters[i].hp = 100;
            hunters[i].def = 5;
            // DO NOT RESET BANNED FLAG
            printf("Stats reset for %s. (Ban status unchanged)\n", name);
            return;
        }
    }
    printf("Hunter not found.\n");
}

void menu() {
    int c;
    while (1) {
        printf("\n=============== SYSTEM MENU ===============\n");
        printf("1. Hunter Info\n");
        printf("2. Dungeon Info\n");
        printf("3. Generate Dungeon\n");
        printf("4. Ban Hunter\n");
        printf("5. Reset Hunter\n");
        printf("6. Exit\n");
        printf("Choice: ");
        scanf("%d", &c); getchar();

        switch (c) {
            case 1: show_hunters(); break;
            case 2: show_dungeons(); break;
            case 3: generate_dungeon(); break;
            case 4: ban_hunter(); break;
            case 5: reset_hunter(); break;
            case 6: detach_and_cleanup(); return;
            default: printf("Invalid choice. Please try again.\n");
        }
    }
}

int main() {
    srand(time(NULL));
    attach_shared_memory();
    menu();
    return 0;
}
