.text

.globl irq50_handler

irq50_handler:
    pusha                  # Sauvegarder tous les registres
    call irq50_handler_c   # Appeler la fonction C associée
    popa                   # Restaurer tous les registres
    iret                   # Retourner de l'interruption