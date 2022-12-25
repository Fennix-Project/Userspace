#include "ld.h"

/* TODO: Implement ELF interpreter. Currently it's implemented in the kernel. */
int main(int argc, char *argv[], char *envp[])
{
    __asm__ __volatile__("syscall"
                         :
                         : "a"(1), "D"('H'), "S"(0)
                         : "rcx", "r11", "memory");
    if (argc < 2)
        return -1;

    int envc = 0;
    while (envp[envc] != NULL)
        envc++;
    return -0xD0D0;
}
