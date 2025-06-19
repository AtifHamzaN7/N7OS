#include <unistd.h>

/*La macro syscall2 permet d'émettre l'interruption*/
syscall2(int, write, const char *, s, int, len);
