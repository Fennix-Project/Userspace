#include "printf.h"

#define AT_NULL 0
#define AT_IGNORE 1
#define AT_EXECFD 2
#define AT_PHDR 3
#define AT_PHENT 4
#define AT_PHNUM 5
#define AT_PAGESZ 6
#define AT_BASE 7
#define AT_FLAGS 8
#define AT_ENTRY 9
#define AT_NOTELF 10
#define AT_UID 11
#define AT_EUID 12
#define AT_GID 13
#define AT_EGID 14
#define AT_PLATFORM 15
#define AT_HWCAP 16
#define AT_CLKTCK 17
#define AT_SECURE 23
#define AT_BASE_PLATFORM 24
#define AT_RANDOM 25
#define AT_HWCAP2 26
#define AT_EXECFN 31
#define AT_SYSINFO 32
#define AT_SYSINFO_EHDR 33
#define AT_L1I_CACHESHAPE 34
#define AT_L1D_CACHESHAPE 35
#define AT_L2_CACHESHAPE 36
#define AT_L3_CACHESHAPE 37
#define AT_L1I_CACHESIZE 40
#define AT_L1I_CACHEGEOMETRY 41
#define AT_L1D_CACHESIZE 42
#define AT_L1D_CACHEGEOMETRY 43
#define AT_L2_CACHESIZE 44
#define AT_L2_CACHEGEOMETRY 45
#define AT_L3_CACHESIZE 46
#define AT_L3_CACHEGEOMETRY 47
#define AT_MINSIGSTKSZ 51

typedef struct
{
    unsigned long a_type;
    union
    {
        unsigned long a_val;
    } a_un;
} Elf64_auxv_t;

static inline long syscall2(int sc, long arg1, long arg2)
{
    long ret;
    __asm__ __volatile__("syscall"
                         : "=a"(ret)
                         : "a"(sc), "D"(arg1), "S"(arg2)
                         : "rcx", "r11", "memory");
    return ret;
}

extern "C" void putchar(char c) { syscall2(1, c, 0); }

int main(int argc, char *argv[], char *envp[])
{
    printf_("Hello World!\n");
    printf_("I have %d arguments\n", argc);
    for (int i = 0; i < argc; i++)
        printf_("argv[%d] = %s\n", i, argv[i]);
    for (int i = 0; envp[i]; i++)
        printf_("envp[%d] = %s\n", i, envp[i]);
    Elf64_auxv_t *auxv;
    while (*envp++ != NULL)
        ;

    int i = 0;
    for (auxv = (Elf64_auxv_t *)envp; auxv->a_type != AT_NULL; auxv++)
    {
        printf_("%lu :%d\n", (auxv->a_type), i++);
    }
    return 0;
}
