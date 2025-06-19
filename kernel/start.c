#include <n7OS/cpu.h>
#include <n7OS/irq.h>
#include <inttypes.h>
#include <n7OS/processor_structs.h>
#include <n7OS/console.h>
#include <n7OS/keyboard.h>
#include <n7OS/mem.h>
#include <n7OS/paging.h>
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <n7OS/processus.h>


extern void irq50_handler();

void kernel_start(void)
{
    init_console();
    clear_console(); 
    kmalloc_init();
    PageDirectory pgdir = initialise_paging();
    setup_base((uint32_t) pgdir);
    init_timer();
    init_syscall();
    init_keyboard();
    sti();
    
    //Test Pagination
    //printf("Hello, paging ok\n");
    alloc_page_entry(0xA0000000, 0, 0);
    uint32_t *ptr = (uint32_t *) 0xA0000000;
    uint32_t do_page_fault = *ptr;
    do_page_fault++;
    
    //Test de la console ( les \n \r \b \r \f ont +été testé : ok)
    //Test Curseur :  console_curseur(0,0); ok
    //Test de la fonction scroll : ok
    
    //Test interruptions 50 : Ok
   // init_irq_entry(50, (uint32_t) irq50_handler);
    // Envoyer une interruption 50
     //__asm__ __volatile__("int $50");

   //test appel exemple 
   //if (example()==1) {
    //   printf ("Appel systeme example ok\n");
   // }

    //test appel shutdown
    //shutdown(1);
    //int v = shutdown(3);
    //printf("Appel systeme shutdown avec parametre 3 : %d\n", v);
    //test appel write
     //write("Test appel systeme write\n", 24);
    // printf("\n Debut des tests processus\n");

    init_processus();
   
    while (1) {
        //test keyboard
        char c = kgetch();
        console_putchar(c);
        // cette fonction arrete le processeur
        hlt();
    }

}
