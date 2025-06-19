#include <n7OS/processus.h>
#include <n7OS/cpu.h>
#include <stdio.h>
#include <string.h>



// J'ai fais le choix de faire un tableau de processus
static Processus table_processus[NB_PROCC_MAX];
static Processus *processus_courant = NULL; //Il va être initialisé dans init_processus
static pid_t compteur_pid = 0;

extern void ctx_sw(void *ctx_old, void *ctx_new);
//J'importe les trois processus pour test, ils sont disponnible dans n7OS/bin
extern void idle();
extern void processus1();
extern void processus2();
//Provenance de time pour gérer le sleep reveil des processus
extern uint32_t system_ticks;

// Creation d'un nouveau processus
pid_t creer_processus(const char *nom, fnptr fonction) {
    //logs pour bien comprendre l'affichage dans le qernel
    printf("[CREER] Creation du processus: %s\n", nom);
    for (int i = 0; i < NB_PROCC_MAX; i++) {
        Processus *proc = &table_processus[compteur_pid];
        if (proc->etat == NONE) {
            proc->pid = compteur_pid;
            proc->etat = PRET;     
            proc->stack[STACK_SIZE - 1] = (uint32_t)fonction;
            proc->regs[1] = (uint32_t)&proc->stack[STACK_SIZE - 1]; // ESP
            strcpy(proc->nom, nom);  
            return compteur_pid++;
        }
        compteur_pid++;
    }
    return -1;
}

void scheduler() {
    if (!processus_courant) return;
    printf("[scheduler] Selection du prochain processus...\n");
    pid_t pid_actuel = getpid();
    pid_t pid_suivant = -1;
    for (pid_t i = 1; i < NB_PROCC_MAX; i++) {
        pid_t test_pid = (pid_actuel + i) % NB_PROCC_MAX;
        Processus *p = &table_processus[test_pid];
        //Recherche d'un processus pret ou reveillé
        if (p->etat == PRET || (p->etat == BLOQUE && system_ticks >= p->temps_reveil)) {
            if (p->etat == BLOQUE) {
                printf("[REVEIL] Processus %s (pid=%u) se reveille\n", p->nom, p->pid);
                p->etat = PRET;
            }
            pid_suivant = test_pid;
            break;
        }
    }
    // Si on a trouvé un processus prêt ou réveillé, on le sélectionne
    if (pid_suivant != -1) {
        printf("[CHANGEMENT] On passe de PID %u a PID %u\n", pid_actuel, pid_suivant);
        if (processus_courant && processus_courant->etat == ELU) {
            processus_courant->etat = PRET;
        }
        table_processus[pid_suivant].etat = ELU;
        Processus *ancien = processus_courant;
        processus_courant = &table_processus[pid_suivant];
        ctx_sw(ancien->regs, processus_courant->regs);
    } else {
        printf("[scheduler] Aucun processus pret pour l'instant \n");
    }
}

// Initialisation du systeme de gestion des processus
void init_processus() {
    printf("[INIT] Initialisation du systeme de processus...\n");

    memset(table_processus, 0, sizeof(table_processus));
    compteur_pid = 0;

    pid_t pid_idle = creer_processus("idle", idle);
    creer_processus("processus1", processus1);
    creer_processus("processus2", processus2);

    processus_courant = &table_processus[pid_idle];
    processus_courant->etat = ELU;

    printf("[INIT] Lancement du processus idle (pid=%u)\n", pid_idle);
    idle(); // ne revient jamais
}

void sleep(int seconds) {
    pid_t pid = getpid();
    Processus *proc = &table_processus[pid];
    if (!processus_courant) return;
    printf("[SLEEP] Processus %s (pid=%u) dort pendant %d secondes\n",
           proc->nom, pid, seconds);
    proc->temps_reveil = system_ticks + (seconds * 1000);  // on a multiplité sec * 1000 car sec en réalité represente des millisecondes
    proc->etat = BLOQUE;
    scheduler();
}

// Met le processus courant en pause (replanification immediate)
void arreter_processus() {
    printf("[ARRET] Processus %s (pid=%u) s'arrete temporairement\n", processus_courant->nom, processus_courant->pid);
    processus_courant->etat = ELU;
    scheduler();
}

// Termine definitivement le processus courant
void terminer_processus() {
    printf("[TERMINER] Fin du processus en cours :  %s (pid=%u)\n", processus_courant->nom, processus_courant->pid);
    processus_courant->etat = NONE; //puisqu'on termine le processus on le "supprime"
    scheduler();
}

// Retourne le PID du processus actuellement elu
pid_t getpid() {
    return processus_courant->pid;
}


pid_t fork() {
    if (!processus_courant) return -1;

    // Cherche une case libre dans la table
    for (int i = 0; i < NB_PROCC_MAX; i++) {
        if (table_processus[i].etat == NONE) {
            Processus *child = &table_processus[i];
            Processus *parent = processus_courant;
            child->pid = compteur_pid++;
            child->etat = PRET;
            snprintf(child->nom, sizeof(child->nom), "%s_fork", parent->nom);
            // Copier pile et registre
            memcpy(child->stack, parent->stack, sizeof(child->stack));
            memcpy(child->regs, parent->regs, sizeof(child->regs));
            child->temps_reveil = 0;
            // Calculer décalage entre les deux piles
            uint32_t offset = (uint32_t)&child->stack[0] - (uint32_t)&parent->stack[0];
            // Adapter les registres empilés (esp, ebp)
            child->regs[1] += offset; // ESP
            child->regs[2] += offset; // EBP
            printf("[FORK] Processus %s forké en %s (pid=%u)\n",
                   parent->nom, child->nom, child->pid);
            return child->pid;
        }
    }
    return -1; 
}

