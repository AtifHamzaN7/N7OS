#include <n7OS/keyboard.h>
#include <n7OS/irq.h>
#include <n7OS/cpu.h>

#define KEYBOARD_IRQ 1
// Buffer circulaire pour les touches
#define KBD_BUF_SIZE 128
static char kbd_buffer[KBD_BUF_SIZE];
static int kbd_buf_head = 0, kbd_buf_tail = 0; 

extern void keyboard_handler();

void init_keyboard() {
    //Demasquer l'IRQ du Clavier
    outb(inb(0x21) & ~(1 << KEYBOARD_IRQ), 0x21);
    //handler pour l'IRQ 1 (interruption 0x21)
    init_irq_entry(0x21, (uint32_t)keyboard_handler);
}


static inline int is_buffer_empty() {
    return kbd_buf_head == kbd_buf_tail;
}

// Table de conversion 
static const char scancode_map[128] = {
    0, 27, '&', 'é', '"', '\'', '(', '-', 'è', '_', 'ç', 'à', ')', '=', '\b',
    '\t', 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n',
    0, 'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'ù', '*', 0,
    0, 'w', 'x', 'c', 'v', 'b', 'n', ',', ';', ':', '!', 0, 0, 0, 0, 0,
};

void buffer_put(char c) { 
    int prochain = (kbd_buf_tail + 1) % KBD_BUF_SIZE;
    if (prochain != kbd_buf_head) { // Une vérification pour éviter de dépasser la taille du buffer
        kbd_buffer[kbd_buf_tail] = c;
        kbd_buf_tail = prochain;
        
    }
}

void keyboard_handler_c() {
    uint8_t scancode = inb(0x60);
    // Si bit 7 à 1, c'est un relâchement, on ignore
    char c = scancode_map[scancode];
    if (scancode & 0x80) {
        // C'est un relâchement de touche, on ignore
    } else { 
        if (c != 0) {
            buffer_put(c);
        }
    }
    // ACK de l'IT clavier
    outb(0x20, 0x20);
}

char kgetch() {
    char c;
    while (is_buffer_empty()) {
        // attente 
    }
    c = kbd_buffer[kbd_buf_head];
    kbd_buf_head = (kbd_buf_head + 1) % KBD_BUF_SIZE;
    return c;
}