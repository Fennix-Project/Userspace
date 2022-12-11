#include <stdlib.h>
#include <stddef.h>

#include "../mem/liballoc_1_1.h"

void abort(void)
{
    while (1)
        ;
}

int atexit(void (*function)(void))
{
    return 0;
}

void exit(int status)
{
    while (1)
        ;
}

int atoi(const char *nptr)
{
    // uint64_t Length = strlen((char *)nptr);
    uint64_t Length = 0;
    if (nptr)
        while (nptr[Length] != '\0')
            ++Length;
    uint64_t OutBuffer = 0;
    uint64_t Power = 1;
    for (uint64_t i = Length; i > 0; --i)
    {
        OutBuffer += (nptr[i - 1] - 48) * Power;
        Power *= 10;
    }
    return OutBuffer;
}

char *getenv(const char *name)
{
    static char *env = "PATH=/bin";
    return env;
}

void *malloc(size_t Size) { return PREFIX(malloc)(Size); }
void *realloc(void *Address, size_t Size) { return PREFIX(realloc)(Address, Size); }
void *calloc(size_t Count, size_t Size) { return PREFIX(calloc)(Count, Size); }
void free(void *Address)
{
    PREFIX(free)
    (Address);
}
