int main(int argc, char *argv[], char *envp[]);

void _start()
{
    register int argc __asm__("rdi");
    register char **argv __asm__("rsi");
    register char **envp __asm__("rdx");
    int mainret = main((int)argc, (char **)argv, (char **)envp);
    __asm__ __volatile__("syscall"
                         :
                         : "a"(0), "D"(mainret)
                         : "rcx", "r11", "memory");
    return;
}
// C++ stuff