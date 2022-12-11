#include <stdlib.h>
#include <init.h>
#include <aux.h>


#define print(m, ...) init_log(m, ##__VA_ARGS__)

int main(int argc, char *argv[], int envc, char *envp[])
{
    print("Hello World!\n");
    print("%p %p %p %p\n", argc, argv, envc, envp);
    print("I have %d arguments\n", argc);
    for (int i = 0; i < argc; i++)
        print("argv[%d] = (%p) %s\n", i, argv[i], argv[i]);

    print("I have %d environment variables\n", envc);
    for (int i = 0; i < envc; i++)
        print("envp[%d] = (%p) %s\n", i, envp[i], envp[i]);
    Elf64_auxv_t *auxv;
    while (*envp++ != NULL)
        ;

    int i = 0;
    for (auxv = (Elf64_auxv_t *)envp; auxv->a_type != AT_NULL; auxv++)
    {
        print("%lu :%d\n", (auxv->a_type), i++);
    }

    // This code somehow breaks the process. How? I can't figure it out if it's a bug in the kernel or in the libc.
    void *yes = malloc(0x100);
    print("malloc(0x100) = %p\n", yes);
    free(yes);
    return 0;
}
