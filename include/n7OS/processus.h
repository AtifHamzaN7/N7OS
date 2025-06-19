#ifndef PROCESSUS_H
#define PROCESSUS_H

#include <inttypes.h>

#define NB_PROCC_MAX 255
#define STACK_SIZE 1024

typedef uint32_t pid_t; // typedef dans le sujet
typedef void (*fnptr)(void); //typedeg suggéré dans l'énoncé

typedef enum {
    NONE = 0, //Cet état est utilisé pour les processus non initialisés dans la fonction creer_processus
    ELU,
    PRET,
    BLOQUE
} etat_t;

typedef struct {
    pid_t pid;
    etat_t etat;
    char nom[18];
    uint32_t regs[5];        // Registres sauvegardés, présenté dans le sujet
    uint32_t stack[STACK_SIZE];
    unsigned long temps_reveil;  // Pour le sleep
} Processus;

// Différentes fonctions pour la gestion des processus
void scheduler();
pid_t creer_processus(const char *nom, fnptr fonction);
void arreter_processus();
void terminer_processus();
pid_t getpid();
//fonction pour le test
void init_processus(void);
//gere le sleep des processus, le reveil est géré par le scheduler
void sleep(int seconds);
//gère le fork 
pid_t fork();


#endif