#include <stdio.h>
#include <n7OS/processus.h>

void processus1() {
    for (int i = 0; i < 5; i++) {
        printf("P1: etape %d\n", i);
        sleep(5);         
    }
    terminer_processus(); // Terminer le processus après les étapes
    
    
}