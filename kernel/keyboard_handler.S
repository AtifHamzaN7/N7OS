.text

.globl keyboard_handler

keyboard_handler:
    pusha
    call keyboard_handler_c
    popa
    iret