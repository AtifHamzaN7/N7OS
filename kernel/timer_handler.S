.text

.globl timer_handler

timer_handler:
    pusha
    call timer_handler_c  # Appeler la fonction C associée
    popa
    iret