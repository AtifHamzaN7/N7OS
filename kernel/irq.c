#include <n7OS/irq.h>
#include <n7OS/processor_structs.h>
#include "inttypes.h"
#include <n7OS/console.h>
#include <stdio.h>


void init_irq_entry(int irq_num, uint32_t handler) {
    idt_entry_t entry;
    entry.offset_inf = (uint16_t)(handler & 0xFFFF);
    entry.sel_segment = KERNEL_CS; // kernel code segment
    entry.zero = 0;
    entry.type_attr = 0x8E; // Présent avec DPL=0 
    entry.offset_sup = (uint16_t)((handler >> 16) & 0xFFFF);

    // Copier l'entrée dans l'IDT
    // 1. Prendre l'adresse de entry avec & : (&entry)
    // 2. Caster cette adresse en pointeur vers unsigned long long avec (unsigned long long *) : (unsigned long long *)&entry
    // 3. Déréférencer ce pointeur pour obtenir la valeur entière 64 bits (*(unsigned long long *)&entry) 
    // 4. Copier cette valeur dans l'entrée correspondante de l'IDT (idt[irq_num])

    idt[irq_num] = *(unsigned long long *)&entry;
    
    
}

void irq50_handler_c() {
    printf("Interruption 50 recue !\n");
}