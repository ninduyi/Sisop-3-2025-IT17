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
int shmid_hunter, shmid_dungeon;

const char *dungeon_names[] = {
    "Double Dungeon", "Demon Castle", "Pyramid Dungeon", "Red Gate Dungeon",
    "Hunters Guild Dungeon", "Busan A-Rank Dungeon", "Insects Dungeon",
    "Goblins Dungeon", "D-Rank Dungeon", "Gwanak Mountain Dungeon",
    "Demon King", "Dragon's Lair", "Underground Dungeon"
};

// Attach shared memory for hunters and dungeons
void attach_shared_memory() {
    shmid_hunter = shmget(SHM_KEY_HUNTER, sizeof(Hunter) * MAX_HUNTERS, IPC_CREAT | 0666);
    shmid_dungeon = shmget(SHM_KEY_DUNGEON, sizeof(Dungeon) * MAX_DUNGEONS, IPC_CREAT | 0666);
    hunters = (Hunter *)shmat(shmid_hunter, NULL, 0);
    dungeons = (Dungeon *)shmat(shmid_dungeon, NULL, 0);
}

// Detach and clean up shared memory when done
void detach_and_cleanup() {
    // Hapus shared memory individual hunter
    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (hunters[i].in_use) {
            int shm_id = shmget(hunters[i].key, sizeof(Hunter), 0666);
            if (shm_id != -1) {
                shmctl(shm_id, IPC_RMID, NULL);
            }
        }
    }

    // Hapus shared memory individual dungeon
    for (int i = 0; i < MAX_DUNGEONS; i++) {
        if (dungeons[i].in_use) {
            int shm_id = shmget(dungeons[i].key, sizeof(Dungeon), 0666);
            if (shm_id != -1) {
                shmctl(shm_id, IPC_RMID, NULL);
            }
        }
    }

    // Detach dan hapus shared memory utama
    shmdt(hunters);
    shmdt(dungeons);

    shmctl(shmid_hunter, IPC_RMID, NULL);
    shmctl(shmid_dungeon, IPC_RMID, NULL);

    printf("Semua shared memory telah dihapus. Exiting system.\n");
}


// Display all hunters' information
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

// Display all dungeons' information
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

// Generate a new dungeon and assign a unique key
void generate_dungeon() {
    for (int i = 0; i < MAX_DUNGEONS; i++) {
        if (!dungeons[i].in_use) {
            // Generate a unique key for the dungeon using time or random value
            long key = time(NULL) + rand();  // Create a unique key based on time and random value

            // Create unique shared memory for each dungeon
            int shm_id = shmget(key, sizeof(Dungeon), IPC_CREAT | 0666);
            if (shm_id == -1) {
                perror("Failed to create shared memory for dungeon");
                return;
            }

            Dungeon *new_dungeon = shmat(shm_id, NULL, 0);
            if (new_dungeon == (void *) -1) {
                perror("Failed to attach shared memory for dungeon");
                return;
            }

            // Fill the dungeon data with unique attributes
            new_dungeon->in_use = 1;
            strncpy(new_dungeon->name, dungeon_names[rand() % 13], NAME_LEN);
            new_dungeon->min_level = rand() % 5 + 1;
            new_dungeon->atk_reward = rand() % 51 + 100;
            new_dungeon->hp_reward = rand() % 51 + 50;
            new_dungeon->def_reward = rand() % 26 + 25;
            new_dungeon->exp_reward = rand() % 151 + 150;
            new_dungeon->key = key;  // Assign a unique key for the dungeon

            // Add dungeon to the main list of dungeons
            dungeons[i] = *new_dungeon;

            printf("\nDungeon generated and added to system:\n");
            printf("Name           : %s\n", new_dungeon->name);
            printf("Minimum Level  : %d\n", new_dungeon->min_level);

            shmdt(new_dungeon); // Detach shared memory after finishing
            return;
        }
    }
    printf("Dungeon memory full.\n");
}

// Ban or unban a hunter
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

// Reset hunter stats
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

// Menu for the system
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
