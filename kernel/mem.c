#include <n7OS/mem.h>
#include <string.h>
#include <stdio.h>

#define NUM_PAGES 4096 // le nombre total de pages pour une mémoire de 16Mo
#define BITMAP_SIZE (NUM_PAGES / 32) // Taille du tableau de bitmap

static uint32_t free_page_bitmap_table[BITMAP_SIZE]; // Utilisation de bitmap, 1 bit = 1 page libre


/**
 * @brief Marque la page allouée
 * 
 * Lorsque la page a été choisie, cette fonction permet de la marquer allouée
 * 
 * @param addr Adresse de la page à allouer
 */
void setPage(uint32_t addr) {
    uint32_t page_num = addr / PAGE_SIZE;
    if (page_num < NUM_PAGES) {
        free_page_bitmap_table[page_num/32] = free_page_bitmap_table[page_num / 32] | (1 << (page_num % 32)); // La page est marquée comme allouée
    }

}

/**
 * @brief Désalloue la page
 * 
 * Libère la page allouée.
 * 
 * @param addr Adresse de la page à libérer
 */
void clearPage(uint32_t addr) {
    uint32_t page_num = addr / PAGE_SIZE;
    if (page_num < NUM_PAGES) {
        free_page_bitmap_table[page_num / 32] = free_page_bitmap_table[page_num / 32] & ~(1 << (page_num % 32)); // La page est marquée comme libre
    }
}

/**
 * @brief Fourni la première page libre de la mémoire physique tout en l'allouant
 * 
 * @return uint32_t Adresse de la page sélectionnée
 */
uint32_t findfreePage() {
    for (uint32_t i = 0; i < BITMAP_SIZE; i++) {
        if (free_page_bitmap_table[i] != 0xFFFFFFFF) { // Si il y a au moins une page libre dans le bitmap
            for (uint32_t j = 0; j < 32; j++) { //On parcourt les bits un à un
                if (!(free_page_bitmap_table[i] & (1 << j))) { // Si la page est libre
                    setPage((i * 32 + j) * PAGE_SIZE); // l'adresse c'est le num_page * taille_page
                    return (i * 32 + j) * PAGE_SIZE; // Retourner l'adresse de la page
                }
            }
        }
    }
    return 0xFFFFFFFF; // Retourner une adresse invalide si aucune page libre n'est trouvée
}

/**
 * @brief Initialise le gestionnaire de mémoire physique
 * 
 */
void init_mem() { 
    memset(free_page_bitmap_table, 0, sizeof(free_page_bitmap_table)); // Initialiser toutes les pages comme libres

}

/**
 * @brief Affiche l'état de la mémoire physique
 * 
 */
void print_mem() {
    for (uint32_t i = 0; i < NUM_PAGES; i++) {
        uint32_t bitmap = free_page_bitmap_table[i / 32];
        uint32_t bit = 1 << (i % 32);
        printf("La page %d: %s\n", i, (bitmap & bit) ? "est allouée" : "est libre");
    }
}