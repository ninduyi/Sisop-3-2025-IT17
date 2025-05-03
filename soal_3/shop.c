#include <stdio.h>
#include <string.h>

// Definisi struktur Weapon
typedef struct {
    char name[50];
    int price;
    int damage;
    char passive[50];
} Weapon;

// Definisi kode warna ANSI
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RESET   "\033[0m"

// Fungsi untuk mendapatkan detail senjata berdasarkan nomor
Weapon get_weapon(int weapon_choice) {
    Weapon weapon;
    switch (weapon_choice) {
        case 1:
            strcpy(weapon.name, "Pedang Lorem Ipsum");
            weapon.price = 50;
            weapon.damage = 10;
            strcpy(weapon.passive, "");
            break;
        case 2:
            strcpy(weapon.name, "Palu Mas Amba");
            weapon.price = 150;
            weapon.damage = 25;
            strcpy(weapon.passive, "");
            break;
        case 3:
            strcpy(weapon.name, "Linggis Jawa");
            weapon.price = 200;
            weapon.damage = 35;
            strcpy(weapon.passive, "");
            break;
        case 4:
            strcpy(weapon.name, "AK47");
            weapon.price = 140;
            weapon.damage = 30;
            strcpy(weapon.passive, "15% Insta-Kill Chance");
            break;
        case 5:
            strcpy(weapon.name, "Es Krim Choco Mint");
            weapon.price = 300;
            weapon.damage = 58;
            strcpy(weapon.passive, "+30% Crit Chance");
            break;
        case 6:
            strcpy(weapon.name, "Keris Hitam Pujo");
            weapon.price = 450;
            weapon.damage = 69;
            strcpy(weapon.passive, "+45% Poisoned");
            break;
        case 7:
            strcpy(weapon.name, "Tongkat Cherybelle");
            weapon.price = 600;
            weapon.damage = 100;
            strcpy(weapon.passive, "+58% Stun Chance");
            break;
        case 8:
            strcpy(weapon.name, "Cakar Berminyak Diddy");
            weapon.price = 750;
            weapon.damage = 1000;
            strcpy(weapon.passive, "25% Insta-Kill Chance");
            break;
        case 9:
            strcpy(weapon.name, "Senapan Lantak Andriana");
            weapon.price = 850;
            weapon.damage = 800;
            strcpy(weapon.passive, "+90% Stun Chance");
            break;
        case 10:
            strcpy(weapon.name, "Sendal Tapok Rai");
            weapon.price = 430;
            weapon.damage = 33;
            strcpy(weapon.passive, "+10% Stun Chance");
            break;
        default:
            strcpy(weapon.name, "Unknown");
            weapon.price = 0;
            weapon.damage = 0;
            strcpy(weapon.passive, "");
            break;
    }
    return weapon;
}

// Fungsi untuk menampilkan daftar senjata di toko
void display_shop(char *buffer, int buffer_size) {
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer),
             "\n"
             COLOR_GREEN "======== WEAPON SHOP ========\n" COLOR_RESET);
    for (int i = 1; i <= 10; i++) {
        Weapon weapon = get_weapon(i);
        if (strlen(weapon.passive) > 0) {
            snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer),
                     "[%d] %-20s - Price: " COLOR_YELLOW "%d Gold" COLOR_RESET ", Damage: " COLOR_RED "%d" COLOR_RESET " (Passive: " COLOR_CYAN "%s" COLOR_RESET ")\n",
                     i, weapon.name, weapon.price, weapon.damage, weapon.passive);
        } else {
            snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer),
                     "[%d] %-20s - Price: " COLOR_YELLOW "%d Gold" COLOR_RESET ", Damage: " COLOR_RED "%d" COLOR_RESET "\n",
                     i, weapon.name, weapon.price, weapon.damage);
        }
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer),
             "Enter weapon number to buy (0 to cancel): ");
}

// Fungsi untuk membeli senjata
int buy_weapon(int weapon_choice, int *gold, char *equipped_weapon, int *base_damage) {
    if (weapon_choice < 1 || weapon_choice > 10) {
        return 0; // Pilihan tidak valid
    }

    Weapon weapon = get_weapon(weapon_choice);
    if (*gold < weapon.price) {
        return -1; // Gold tidak cukup
    }

    *gold -= weapon.price;
    strcpy(equipped_weapon, weapon.name);
    *base_damage = weapon.damage;
    return 1; // Pembelian berhasil
}