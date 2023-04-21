#include <stdlib.h>
#include <aux.h>

#include <sysbase.h>
#include <sysfile.h>
#include <init.h>

void PutCharToKernelConsole(char c)
{
    __asm__ __volatile__("syscall"
                         :
                         : "a"(1), "D"(c), "S"(0)
                         : "rcx", "r11", "memory");
}

#define print(m, ...) init_log(m, ##__VA_ARGS__)

int main(int argc, char *argv[], char *envp[])
{
    print("%p %p %p\n", (void *)(uint64_t)&argc, (void *)&argv, (void *)&envp);
    print("I have %d arguments\n", argc);
    for (int i = 0; i < argc; i++)
        print("argv[%d] = (%p) %s\n", i, argv[i], argv[i]);

    int envc = 0;
    while (envp[envc] != NULL)
        envc++;

    print("I have %d environment variables\n", envc);
    for (int i = 0; i < envc; i++)
        print("envp[%d] = (%p) %s\n", i, envp[i], envp[i]);

    Elf64_auxv_t *auxv;
    char **e = envp;

    while (*e++ != NULL)
        ;

    for (auxv = (Elf64_auxv_t *)e; auxv->a_type != AT_NULL; auxv++)
        print("auxv: %ld %#lx\n", auxv->a_type, auxv->a_un.a_val);

    File *test = FileOpen("/Test.txt", FILE_READ);
    if (test == NULL)
    {
        print("Failed to open file\n");
        return 1;
    }

    char buf[1024];
    uint64_t read = FileRead(test, 0, (uint8_t *)buf, 1024);
    print("Read %ld bytes from file\n", read);
    print("File contents: %s\n", buf);
    FileClose(test);
    return 0;
}
