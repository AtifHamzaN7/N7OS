#include <n7OS/console.h>
#include <n7OS/cpu.h>
#include <string.h>
#define Couleur_Ecran 0xF0 // fond blanc
uint16_t *scr_tab;

int curseurx = 0;
int curseury = 0;
static int indice_couleur = 0; // Compteur pour les couleurs

void init_console() {
   // scr_tab= (uint16_t *) SCREEN_ADDR;
    //for (int i = 0; i < 80 * 25; i++) { //Fond Blanc
    //    scr_tab[i] = (Couleur_Ecran << 8) | ' ';
    scr_tab= (uint16_t *) SCREEN_ADDR;
    //}    
}

void clear_console() {
    // Efface l’écran et revient à la colonne 0 de la ligne 0
    for (int i = 0; i < 80 * 25; i++) { // l'écran est un tableau de 80*25 cases
        scr_tab[i] = (Couleur_Ecran << 8) | ' ';
    }
    curseurx = 0;
    curseury = 0;
    console_curseur(curseurx, curseury);
}

const uint8_t colors[] = {
    0x0, // noir
    0x1, // bleu
    0x2, // vert
    0x3, // cyan
    0x4, // rouge
    0x5, // magenta
    0x6, // marron
    0x7, // gris clair
    0x8, // gris foncé
    0x9, // bleu clair
    0xA, // vert clair
    0xB, // cyan clair
    0xC, // rouge clair
    0xD, // magenta clair
};

void animation() { //une petite animation sympa au lancement de la console
    for (int step = 0; step < 12; step++) { 
        for (int i = 0; i < 25; i++) { 
            int rand_col = (step * 7 + i * 3) % 80; 
            int rand_color = colors[(step + i) % (sizeof(colors) / sizeof(colors[0]))]; 
            for (int j = 0; j < 10; j++) {
                scr_tab[i * 80 + (rand_col + j) % 80] = (rand_color << 8) | '#';
            }
        }
        for (volatile int i = 0; i < 50000000; i++);
    }
    // Effacer tout après l'effet
    for (int i = 0; i < 80 * 25; i++) {
        scr_tab[i] = (Couleur_Ecran << 8) | ' ';
    }
}



void scroll() {
    // Déplace toutes les lignes vers le haut
    memcpy(scr_tab, scr_tab + 80, 80 * 24 * sizeof(uint16_t));
    // Efface la dernière ligne
    for (int x = 0; x < 80; x++) {
        scr_tab[24 * 80 + x] = (Couleur_Ecran << 8) | ' ';
    }
    curseury = 24;
}


void console_putchar(const char c) {
    if (c > 31 && c < 127) {
        // on affiche le caractère ( couleur modulo pour avoir diff couleur)
        int pos = 80 * curseury + curseurx;
        uint8_t color = colors[indice_couleur % (sizeof(colors) / sizeof(colors[0]))];
        scr_tab[pos] = (Couleur_Ecran | color) << 8 | c;
        curseurx++;
        indice_couleur++;
        if (curseurx >= 80) {
            curseurx = 0;
            curseury++;
        }
        if (curseury >= 25) {
            scroll();
        }

        
    } else if ( c == '\n') {
       // Déplace le curseur à la ligne suivante, colone 0
       curseurx = 0;
       curseury++;
       if (curseury >= 25) {
        scroll();
       }
    } else if (c == '\t') {
       // Ajoute un espace de 8 caractères
       curseurx += 8;
        if (curseurx >= 80) {
            curseurx = 0;
            curseury++;
        }
        if (curseury >= 25) {
            scroll();
        }
    } else if (c == '\b') {
        // Déplace le curseur d’une colonne en arrière
        if (curseurx > 0) {
            curseurx--;
        } else if (curseury > 0) { //si on est en colonne 0, on remonte d'une ligne et on se place en colonne 79
            curseury--;
            curseurx = 79;
        }
    } else if (c == '\r') {
       // Déplace le curseur à la colonne 0 de la ligne courante
         curseurx = 0;
     } else if (c == '\f') {
        // Efface l’écran et revient à la colonne 0 de la ligne 0
        for (int i = 0; i < 80 * 25; i++) { // l'écran est un tableau de 80*25 cases
            scr_tab[i] = 0;
        }
        curseurx = 0;
        curseury = 0;
     }
     // Met à jour la position du curseur
    console_curseur(curseurx, curseury);
}

void console_putbytes(const char *s, int len) {
    for (int i= 0; i<len; i++) {
        console_putchar(s[i]);
    }
}
void console_curseur(int x,int y) {
    //x compris entre 0 et 79 (80 colonnes), et y compris en 0 et 24 (25 lignes)
    uint16_t pos= 80 * y + x;
    curseurx = x;
    curseury = y;
    outb(0xF, 0x3D4); // envoi de la commande 15 (0xF)
    outb((uint8_t) (pos & 0xFF), 0x3D5); // envoi de la valeur du poids faible de la position
    outb(0xE, 0x3D4); // envoi de la commande 14 (0xE)
    outb((uint8_t) ((pos >> 8) & 0xFF), 0x3D5); //envoi de la valeur du poids fort de la position
   
}

