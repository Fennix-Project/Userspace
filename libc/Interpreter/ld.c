#include "ld.h"

int main(int argc, char *argv[], char *envp[])
{
    if (argc < 2)
        return -1;

    int envc = 0;
    while (envp[envc] != NULL)
        envc++;
    return -0xD0D0;
}
