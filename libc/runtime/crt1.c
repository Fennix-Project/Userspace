int main(int argc, char *argv[], char *envp[]);

void _start()
{
    // TODO: Implement
    int *argc = 0;
    char **argv = 0;
    char **envp = 0;
    int mainret = main((int)*argc, (char **)argv, (char **)envp);
    __asm__ __volatile__("syscall"
                         :
                         : "a"(0), "D"(mainret)
                         : "rcx", "r11", "memory");
    return;
}
// C++ stuff