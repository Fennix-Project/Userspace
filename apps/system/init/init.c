#include <stdlib.h>
#include <init.h>
#include <aux.h>
#include <syslib.h>

#define print(m, ...) init_log(m, ##__VA_ARGS__)

int main(int argc, char *argv[], char *envp[])
{
    print("Hello World!\n");
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

    return 0;
}
