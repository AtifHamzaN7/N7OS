#include <unistd.h>

/*La macro syscall1 permet d’émettre l’interruption logicielle 0x80.*/
syscall1(int, shutdown, int, n);
