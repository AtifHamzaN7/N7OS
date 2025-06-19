#include <stdio.h>
#include <n7OS/processus.h>

void processus2() {
    for (int i = 0; i < 5; i++) {
        printf("P2: etape %d\n", i);
        sleep(3);  
    }
    terminer_processus(); // Terminer le processus après les étapes
   
}