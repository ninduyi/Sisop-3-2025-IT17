#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 2048
#define MAX_INVENTORY 10
#define HEALTH_BAR_WIDTH 20

// Kode warna ANSI
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RESET   "\033[0m"

// Definisi struktur Weapon
typedef struct {
    char name[50];
    int price;
    int damage;
    char passive[50];
} Weapon;

// Deklarasi eksternal untuk fungsi dari shop.c
extern void display_shop(char *buffer, int buffer_size);
extern int buy_weapon(int weapon_choice, int *gold, char *equipped_weapon, int *base_damage);
extern Weapon get_weapon(int weapon_choice);

typedef struct {
    int gold;
    Weapon inventory[MAX_INVENTORY];
    int inventory_count;
    int equipped_weapon_index;
    int kills;
} Player;

typedef struct {
    int hp;
    int max_hp;
    int poisoned_turns; // Untuk melacak berapa turn musuh terkena poison
    int stunned;       // Untuk melacak apakah musuh sedang stunned
} Enemy;

void *handle_client(void *client_socket) {
    int sock = *(int *)client_socket;
    char buffer[BUFFER_SIZE];
    int read_size;
    Player player = {0, {}, 0, -1, 0}; // Gold awal 0
    
    // Inisialisasi inventaris awal dengan Fists
    strcpy(player.inventory[0].name, "Fists");
    player.inventory[0].damage = 5;
    player.inventory[0].price = 0;
    strcpy(player.inventory[0].passive, "");
    player.inventory_count = 1;
    player.equipped_weapon_index = 0;

    int shop_active = 0;
    int inventory_active = 0;
    int battle_active = 0;
    Enemy enemy = {0, 0, 0, 0};

    char *welcome = "\n"
                    COLOR_CYAN

"  _/////    _//     _//_///     _//   _////   _////////    _////     _///     _//  _// //  _////////  _// //        _/       _/// _////// \n"
"  _//   _// _//     _//_/ _//   _// _/    _// _//        _//    _//  _/ _//   _//_//    _//_//      _//    _//     _/ //          _//      \n"
"  _//    _//_//     _//_// _//  _//_//        _//      _//        _//_// _//  _// _//      _//       _//          _/  _//         _//      \n"
"  _//    _//_//     _//_//  _// _//_//        _//////  _//        _//_//  _// _//   _//    _//////     _//       _//   _//        _//       \n"
"  _//    _//_//     _//_//   _/ _//_//   _////_//      _//        _//_//   _/ _//      _// _//            _//   _////// _//       _//       \n" 
"  _//   _// _//     _//_//    _/ // _//    _/ _//        _//     _// _//    _/ //_//    _//_//      _//    _// _//       _//      _//       \n"
"  _/////      _/////   _//      _//  _/////   _////////    _////     _//      _//  _// //  _////////  _// //  _//         _//     _//       \n"
"                                                                                                                                           \n"


                    "=====================================\n"
                    "   SELAMAT DATANG DI LOST DUNGEON   \n"
                    "=====================================\n" COLOR_RESET
                    " Kau berdiri di tengah dungeon kuno.\n"
                    " Di dekatmu ada toko senjata dan pintu seram...\n\n"
                    COLOR_YELLOW "========== MAIN MENU ==========\n" COLOR_RESET
                    "  1. Show Player Stats\n"
                    "  2. Shop (Buy Weapons)\n"
                    "  3. View Inventory & Equip Weapons\n"
                    "  4. Battle Mode\n"
                    "  5. Exit Game\n\n"
                    "Choose an option: ";
    send(sock, welcome, strlen(welcome), 0);

    while ((read_size = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0'; // Pastikan buffer diakhiri dengan null
        // Hapus karakter newline atau whitespace di akhir
        for (int i = 0; i < read_size; i++) {
            if (buffer[i] == '\n' || buffer[i] == '\r') {
                buffer[i] = '\0';
                break;
            }
        }
        char response[BUFFER_SIZE];

        if (battle_active) {
            // Prioritaskan perintah 'exit' agar langsung diproses
            if (strcmp(buffer, "exit") == 0) {
                battle_active = 0;
                snprintf(response, BUFFER_SIZE,
                         "\n"
                         "  Kamu meninggalkan pertarungan.\n\n"
                         COLOR_YELLOW "========== MAIN MENU ==========\n" COLOR_RESET
                         "  1. Show Player Stats\n"
                         "  2. Shop (Buy Weapons)\n"
                         "  3. View Inventory & Equip Weapons\n"
                         "  4. Battle Mode\n"
                         "  5. Exit Game\n\n"
                         "Choose an option: ");
            }
            else if (strcmp(buffer, "attack") == 0) {
                Weapon equipped = player.inventory[player.equipped_weapon_index];
                int base_damage = equipped.damage;
                int random_bonus = rand() % 10; // Bonus acak 0-9
                int damage = base_damage + random_bonus;

                // Reset response buffer
                response[0] = '\0';

                // Tentukan peluang critical hit
                int crit_chance = 10; // Default 10%
                if (strstr(equipped.passive, "Crit Chance")) {
                    sscanf(equipped.passive, "+%d%% Crit Chance", &crit_chance);
                }
                int is_critical = (rand() % 100) < crit_chance;

                // Tentukan peluang insta-kill
                int insta_kill_chance = 0;
                if (strstr(equipped.passive, "Insta-Kill Chance")) {
                    sscanf(equipped.passive, "%d%% Insta-Kill Chance", &insta_kill_chance);
                }
                int is_insta_kill = (rand() % 100) < insta_kill_chance;

                // Tentukan peluang poisoned
                int poison_chance = 0;
                if (strstr(equipped.passive, "Poisoned")) {
                    sscanf(equipped.passive, "+%d%% Poisoned", &poison_chance);
                }
                int is_poisoned = (rand() % 100) < poison_chance;

                // Tentukan peluang stun
                int stun_chance = 0;
                if (strstr(equipped.passive, "Stun Chance")) {
                    sscanf(equipped.passive, "+%d%% Stun Chance", &stun_chance);
                }
                int is_stunned = (rand() % 100) < stun_chance;

                // Proses efek poisoned dari turn sebelumnya
                if (enemy.poisoned_turns > 0) {
                    int poison_damage = 5; // Damage per turn akibat poison
                    enemy.hp -= poison_damage;
                    snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                             "\n"
                             COLOR_MAGENTA "  Enemy takes %d damage from poison!\n" COLOR_RESET,
                             poison_damage);
                    enemy.poisoned_turns--;
                }

                // Proses efek stunned
                if (enemy.stunned) {
                    snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                             "\n"
                             COLOR_YELLOW "  Enemy is stunned and cannot attack!\n" COLOR_RESET);
                    enemy.stunned = 0; // Stun hanya berlangsung 1 turn
                } else {
                    // Proses serangan normal
                    if (is_insta_kill) {
                        int reward = (rand() % 100) + 50; // Reward gold acak 50-149
                        player.gold += reward;
                        player.kills += 1;
                        snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                 "\n"
                                 COLOR_RED "=== INSTANT ANNIHILATION! ===\n" COLOR_RESET);
                        if (strcmp(equipped.name, "AK47") == 0) {
                            snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                     "  Your " COLOR_YELLOW "%s" COLOR_RESET " unleashed tons of bullets! Enemy was instantly shot down! DOR DOR DOR DOR!! MODARRR!!\n",
                                     equipped.name);
                        } else if (strcmp(equipped.name, "Cakar Berminyak Diddy") == 0) {
                            snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                     "  Your " COLOR_YELLOW "%s" COLOR_RESET " slashed with deadly precision! Enemy was torn apart!\n",
                                     equipped.name);
                        }
                        snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                 "\n"
                                 COLOR_GREEN "========== REWARD ==========\n" COLOR_RESET
                                 "  You earned " COLOR_YELLOW "%d gold" COLOR_RESET "!\n\n"
                                 COLOR_MAGENTA "======== NEW ENEMY =========\n" COLOR_RESET,
                                 reward);
                        enemy.hp = (rand() % 151) + 50; // HP musuh baru acak 50-200
                        enemy.max_hp = enemy.hp;
                        enemy.poisoned_turns = 0;
                        enemy.stunned = 0;
                    } else {
                        if (is_critical) {
                            damage *= 2;
                            snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                     "\n"
                                     COLOR_RED "=== CRITICAL STRIKE! ===\n" COLOR_RESET);
                            if (strcmp(equipped.name, "Es Krim Choco Mint") == 0) {
                                snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                         "  Your " COLOR_YELLOW "%s" COLOR_RESET " froze the enemy with a chilling strike! CHOCO-MINTO:3 BURIMO ana-taaa!!\n",
                                         equipped.name);
                            }
                        }

                        // Pesan damage selalu ditampilkan
                        snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                 "  You dealt " COLOR_YELLOW "%d damage" COLOR_RESET "!\n",
                                 damage);

                        // Proses efek poisoned
                        if (is_poisoned && enemy.hp > 0) {
                            enemy.poisoned_turns = 3; // Poison berlangsung selama 3 turn
                            snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                     "\n"
                                     COLOR_MAGENTA "=== ENEMY WAS POISONED! ===\n" COLOR_RESET
                                     "  Your " COLOR_YELLOW "%s" COLOR_RESET " poisoned the enemy! They will take damage over time! mengerikan.....\n" COLOR_RESET,
                                     equipped.name);
                        }

                        // Proses efek stun
                        if (is_stunned && enemy.hp > 0) {
                            enemy.stunned = 1; // Stun berlangsung 1 turn
                            snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                     "\n"
                                     COLOR_YELLOW "=== ENEMY STUNNED! ===\n" COLOR_RESET);
                            if (strcmp(equipped.name, "Tongkat Cherybelle") == 0) {
                                snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                         "  Your " COLOR_YELLOW "%s" COLOR_RESET " dazzled the enemy with a magic spark! Enemy is stunned! baby i love you....love you...love you so much <3 !\n" COLOR_RESET,
                                         equipped.name);
                            } else if (strcmp(equipped.name, "Senapan Lantak Andriana") == 0) {
                                snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                         "  Your " COLOR_YELLOW "%s" COLOR_RESET " fired a deafening shot! Enemy is stunned! RODOK! RODOK! RODOK!\n" COLOR_RESET,
                                         equipped.name);
                            } else if (strcmp(equipped.name, "Sendal Tapok Rai") == 0) {
                                snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                         "  Your " COLOR_YELLOW "%s" COLOR_RESET " slapped the enemy with a mighty blow! Enemy is stunned! PLAKKKKK!!!\n" COLOR_RESET,
                                         equipped.name);
                            }
                        }

                        enemy.hp -= damage;
                        if (enemy.hp <= 0) {
                            int reward = (rand() % 100) + 50; // Reward gold acak 50-149
                            player.gold += reward;
                            player.kills += 1;
                            snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                     "  You defeated the enemy!\n\n"
                                     COLOR_GREEN "========== REWARD ==========\n" COLOR_RESET
                                     "  You earned " COLOR_YELLOW "%d gold" COLOR_RESET "!\n\n"
                                     COLOR_MAGENTA "======== NEW ENEMY =========\n" COLOR_RESET,
                                     reward);
                            enemy.hp = (rand() % 151) + 50; // HP musuh baru acak 50-200
                            enemy.max_hp = enemy.hp;
                            enemy.poisoned_turns = 0;
                            enemy.stunned = 0;
                        }
                    }

                    // Hitung persentase HP musuh
                    float hp_percentage = (float)(enemy.hp > 0 ? enemy.hp : 0) / enemy.max_hp * 100;
                    const char *bar_color;
                    if (hp_percentage > 50) {
                        bar_color = COLOR_GREEN;
                    } else if (hp_percentage > 30) {
                        bar_color = COLOR_YELLOW;
                    } else {
                        bar_color = COLOR_RED;
                    }

                    // Tampilkan status musuh dengan health bar berwarna
                    snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                             COLOR_BLUE "======== ENEMY STATUS ========\n" COLOR_RESET
                             "  Enemy health: %s[", bar_color);
                    int bar_width = (enemy.hp > 0) ? (enemy.hp * HEALTH_BAR_WIDTH) / enemy.max_hp : 0;
                    for (int i = 0; i < HEALTH_BAR_WIDTH; i++) {
                        if (i < bar_width) {
                            snprintf(response + strlen(response), BUFFER_SIZE - strlen(response), "█");
                        } else {
                            snprintf(response + strlen(response), BUFFER_SIZE - strlen(response), " ");
                        }
                    }
                    snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                             "]%s " COLOR_RED "%d/%d HP\n" COLOR_RESET
                             "  Type " COLOR_CYAN "'attack'" COLOR_RESET " to attack or " COLOR_CYAN "'exit'" COLOR_RESET " to leave battle.\n\n> ",
                             COLOR_RESET, enemy.hp > 0 ? enemy.hp : 0, enemy.max_hp);
                }
            } else {
                snprintf(response, BUFFER_SIZE,
                         "\n"
                         COLOR_RED "  Pilihan tidak valid!" COLOR_RESET " Type " COLOR_CYAN "'attack'" COLOR_RESET " to attack or " COLOR_CYAN "'exit'" COLOR_RESET " to leave battle.\n\n> ");
            }
        } else if (shop_active) {
            if (strcmp(buffer, "2") != 0) {
                if (strcmp(buffer, "0") == 0) {
                    shop_active = 0;
                    snprintf(response, BUFFER_SIZE,
                             "\n"
                             COLOR_YELLOW "========== MAIN MENU ==========\n" COLOR_RESET
                             "  1. Show Player Stats\n"
                             "  2. Shop (Buy Weapons)\n"
                             "  3. View Inventory & Equip Weapons\n"
                             "  4. Battle Mode\n"
                             "  5. Exit Game\n\n"
                             "Choose an option: ");
                } else {
                    int weapon_choice = atoi(buffer);
                    char temp_weapon[50];
                    int temp_damage;
                    int result = buy_weapon(weapon_choice, &player.gold, temp_weapon, &temp_damage);
                    if (result == 1) {
                        Weapon new_weapon = get_weapon(weapon_choice);
                        if (player.inventory_count < MAX_INVENTORY) {
                            player.inventory[player.inventory_count] = new_weapon;
                            player.inventory_count++;
                            player.equipped_weapon_index = player.inventory_count - 1;
                        }
                        snprintf(response, BUFFER_SIZE,
                                 "\n"
                                 COLOR_GREEN "  Kamu membeli %s!" COLOR_RESET " Gold tersisa: " COLOR_YELLOW "%d\n" COLOR_RESET
                                 "  Senjata otomatis dipakai.\n\n"
                                 COLOR_YELLOW "========== MAIN MENU ==========\n" COLOR_RESET
                                 "  1. Show Player Stats\n"
                                 "  2. Shop (Buy Weapons)\n"
                                 "  3. View Inventory & Equip Weapons\n"
                                 "  4. Battle Mode\n"
                                 "  5. Exit Game\n\n"
                                 "Choose an option: ",
                                 new_weapon.name, player.gold);
                        shop_active = 0;
                    } else if (result == -1) {
                        snprintf(response, BUFFER_SIZE,
                                 "\n"
                                 COLOR_RED "  Gold tidak cukup!\n" COLOR_RESET);
                        display_shop(response, BUFFER_SIZE);
                    } else {
                        snprintf(response, BUFFER_SIZE,
                                 "\n"
                                 COLOR_RED "  Pilihan tidak valid!\n" COLOR_RESET);
                        display_shop(response, BUFFER_SIZE);
                    }
                }
            } else {
                response[0] = '\0';
                display_shop(response, BUFFER_SIZE);
            }
        } else if (inventory_active) {
            if (strcmp(buffer, "") == 0) {
                inventory_active = 0;
                snprintf(response, BUFFER_SIZE,
                         "\n"
                         COLOR_YELLOW "========== MAIN MENU ==========\n" COLOR_RESET
                         "  1. Show Player Stats\n"
                         "  2. Shop (Buy Weapons)\n"
                         "  3. View Inventory & Equip Weapons\n"
                         "  4. Battle Mode\n"
                         "  5. Exit Game\n\n"
                         "Choose an option: ");
            } else {
                int equip_choice = atoi(buffer);
                if (equip_choice >= 0 && equip_choice < player.inventory_count) {
                    player.equipped_weapon_index = equip_choice;
                    snprintf(response, BUFFER_SIZE,
                             "\n"
                             COLOR_GREEN "  Kamu memakai %s.\n" COLOR_RESET
                             COLOR_MAGENTA "======== YOUR INVENTORY ========\n" COLOR_RESET);
                    for (int i = 0; i < player.inventory_count; i++) {
                        if (strlen(player.inventory[i].passive) > 0) {
                            snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                     "  [%d] %-20s (Passive: %-20s)%s\n",
                                     i, player.inventory[i].name, player.inventory[i].passive,
                                     i == player.equipped_weapon_index ? COLOR_GREEN " (EQUIPPED)" COLOR_RESET : "");
                        } else {
                            snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                     "  [%d] %-20s%s\n",
                                     i, player.inventory[i].name,
                                     i == player.equipped_weapon_index ? COLOR_GREEN " (EQUIPPED)" COLOR_RESET : "");
                        }
                    }
                    snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                             "\n  Enter number to equip (Enter to return to menu): ");
                } else {
                    snprintf(response, BUFFER_SIZE,
                             "\n"
                             COLOR_RED "  Pilihan tidak valid!\n" COLOR_RESET
                             COLOR_MAGENTA "======== YOUR INVENTORY ========\n" COLOR_RESET);
                    for (int i = 0; i < player.inventory_count; i++) {
                        if (strlen(player.inventory[i].passive) > 0) {
                            snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                     "  [%d] %-20s (Passive: %-20s)%s\n",
                                     i, player.inventory[i].name, player.inventory[i].passive,
                                     i == player.equipped_weapon_index ? COLOR_GREEN " (EQUIPPED)" COLOR_RESET : "");
                        } else {
                            snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                     "  [%d] %-20s%s\n",
                                     i, player.inventory[i].name,
                                     i == player.equipped_weapon_index ? COLOR_GREEN " (EQUIPPED)" COLOR_RESET : "");
                        }
                    }
                    snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                             "\n  Enter number to equip (Enter to return to menu): ");
                }
            }
        } else {
            int option = atoi(buffer);
            if (option < 1 || option > 5) {
                snprintf(response, BUFFER_SIZE,
                         "\n"
                         COLOR_RED "  Invalid option. Please try again.\n" COLOR_RESET
                         COLOR_YELLOW "========== MAIN MENU ==========\n" COLOR_RESET
                         "  1. Show Player Stats\n"
                         "  2. Shop (Buy Weapons)\n"
                         "  3. View Inventory & Equip Weapons\n"
                         "  4. Battle Mode\n"
                         "  5. Exit Game\n\n"
                         "Choose an option: ");
            } else if (option == 1) {
                Weapon equipped = player.inventory[player.equipped_weapon_index];
                if (strlen(equipped.passive) > 0) {
                    snprintf(response, BUFFER_SIZE,
                             "\n"
                             COLOR_BLUE "======== PLAYER STATS ========\n" COLOR_RESET
                             "  GOLD: " COLOR_YELLOW "%d" COLOR_RESET "  |  EQUIPPED WEAPON: " COLOR_GREEN "%s" COLOR_RESET "  |  BASE DAMAGE: " COLOR_YELLOW "%d" COLOR_RESET "  |  KILLS: " COLOR_YELLOW "%d" COLOR_RESET "  |  PASSIVE: " COLOR_CYAN "%s\n" COLOR_RESET
                             COLOR_YELLOW "========== MAIN MENU ==========\n" COLOR_RESET
                             "  1. Show Player Stats\n"
                             "  2. Shop (Buy Weapons)\n"
                             "  3. View Inventory & Equip Weapons\n"
                             "  4. Battle Mode\n"
                             "  5. Exit Game\n\n"
                             "Choose an option: ",
                             player.gold, equipped.name, equipped.damage, player.kills, equipped.passive);
                } else {
                    snprintf(response, BUFFER_SIZE,
                             "\n"
                             COLOR_BLUE "======== PLAYER STATS ========\n" COLOR_RESET
                             "  GOLD: " COLOR_YELLOW "%d" COLOR_RESET "  |  EQUIPPED WEAPON: " COLOR_GREEN "%s" COLOR_RESET "  |  BASE DAMAGE: " COLOR_YELLOW "%d" COLOR_RESET "  |  KILLS: " COLOR_YELLOW "%d\n" COLOR_RESET
                             COLOR_YELLOW "========== MAIN MENU ==========\n" COLOR_RESET
                             "  1. Show Player Stats\n"
                             "  2. Shop (Buy Weapons)\n"
                             "  3. View Inventory & Equip Weapons\n"
                             "  4. Battle Mode\n"
                             "  5. Exit Game\n\n"
                             "Choose an option: ",
                             player.gold, equipped.name, equipped.damage, player.kills);
                }
            } else if (option == 2) {
                shop_active = 1;
                response[0] = '\0';
                display_shop(response, BUFFER_SIZE);
            } else if (option == 3) {
                inventory_active = 1;
                snprintf(response, BUFFER_SIZE,
                         "\n"
                         COLOR_MAGENTA "======== YOUR INVENTORY ========\n" COLOR_RESET);
                for (int i = 0; i < player.inventory_count; i++) {
                    if (strlen(player.inventory[i].passive) > 0) {
                        snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                 "  [%d] %-20s (Passive: %-20s)%s\n",
                                 i, player.inventory[i].name, player.inventory[i].passive,
                                 i == player.equipped_weapon_index ? COLOR_GREEN " (EQUIPPED)" COLOR_RESET : "");
                    } else {
                        snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                                 "  [%d] %-20s%s\n",
                                 i, player.inventory[i].name,
                                 i == player.equipped_weapon_index ? COLOR_GREEN " (EQUIPPED)" COLOR_RESET : "");
                    }
                }
                snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                         "\n  Enter number to equip (Enter to return to menu): ");
            } else if (option == 4) {
                battle_active = 1;
                enemy.hp = (rand() % 151) + 50; // HP musuh baru acak 50-200
                enemy.max_hp = enemy.hp;
                enemy.poisoned_turns = 0;
                enemy.stunned = 0;
                snprintf(response, BUFFER_SIZE,
                         "\n"
                         COLOR_RED "======== BATTLE STARTED ========\n" COLOR_RESET
                         "  Enemy appeared with: " COLOR_GREEN "[");
                int bar_width = HEALTH_BAR_WIDTH;
                for (int i = 0; i < HEALTH_BAR_WIDTH; i++) {
                    snprintf(response + strlen(response), BUFFER_SIZE - strlen(response), "█");
                }
                snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
                         "]" COLOR_RESET " " COLOR_RED "%d/%d HP\n" COLOR_RESET
                         "  Type " COLOR_CYAN "'attack'" COLOR_RESET " to attack or " COLOR_CYAN "'exit'" COLOR_RESET " to leave battle.\n\n> ",
                         enemy.hp, enemy.max_hp);
            } else if (option == 5) {
                snprintf(response, BUFFER_SIZE,
                         "\n"
                         COLOR_CYAN "  Kamu keluar dari dungeon. Sampai jumpa!\n" COLOR_RESET);
                send(sock, response, strlen(response), 0);
                break;
            }
        }
        send(sock, response, strlen(response), 0);
    }

    close(sock);
    free(client_socket);
    return NULL;
}

int main() {
    srand(time(NULL)); // Inisialisasi seed untuk random
    int server_sock, client_sock, *new_sock;
    struct sockaddr_in server, client;
    pthread_t client_thread;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Gagal membuat socket");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind gagal");
        exit(1);
    }

    listen(server_sock, MAX_CLIENTS);
    printf("Server berjalan, menunggu koneksi...\n");

    int c = sizeof(struct sockaddr_in);
    while ((client_sock = accept(server_sock, (struct sockaddr *)&client, (socklen_t *)&c))) {
        printf("Koneksi baru diterima\n");
        new_sock = malloc(sizeof(int));
        *new_sock = client_sock;
        if (pthread_create(&client_thread, NULL, handle_client, (void *)new_sock) < 0) {
            perror("Gagal membuat thread");
            free(new_sock);
            continue;
        }
        pthread_detach(client_thread);
    }

    close(server_sock);
    return 0;
}