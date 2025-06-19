#include <n7OS/irq.h>
#include <n7OS/console.h>
#include <stdio.h>
#include <inttypes.h>
#include <n7OS/cpu.h>
#include <n7OS/processus.h>

#define FREQUENCE_BASE 1190000 // 1.19 MHZ indiqué en sujet
#define HORLOGE 1000  // Fréquence cible (1 kHz)

extern uint32_t system_ticks = 0; // Compteur système en millisecondes
extern void timer_handler();
extern void scheduler();

void init_timer() {
    uint16_t frequence = FREQUENCE_BASE / HORLOGE;

    //  Impulsion à expiration d’un timer (0011 0110) 110 => Mode 2
    outb(0x36, 0x43);

    // Envoyer le frequence (partie basse puis partie haute)
    outb((uint8_t)(frequence & 0xFF), 0x40 ); //8 bits de poids faibles
    outb((uint8_t)((frequence >> 8) & 0xFF), 0x40); //8 bits de poids forts
    // Timer est numéro d'iT 0x20 en hexa => 32 en décimal
    init_irq_entry(32, (uint32_t)timer_handler);
    
    outb(inb(0x21)&0xfe, 0x21);
}

void timer_handler_c() {
    // ack de l’IT du PIC Master
    outb(0x20, 0x20); 
    // Afficher en haut à droite de la console
    if ( system_ticks % 1000 == 0) { 
        display_system_time();
    }
    system_ticks++; 
    if (system_ticks % 3000 == 0) {
       scheduler();
    }
}

void display_system_time() {
    int saved_x = curseurx;
    int saved_y = curseury;
    uint32_t hours, minutes, seconds;
    seconds = (system_ticks / 1000) % 60;
    minutes = (system_ticks / (1000 * 60)) % 60;
    hours = (system_ticks / (1000 * 60 * 60)) % 24;
    console_curseur(72,0);
    // Afficher directement le temps au format hh:mm:ss
    printf("%02d:%02d:%02d\n", hours, minutes, seconds);
    console_curseur(saved_x, saved_y);  
}