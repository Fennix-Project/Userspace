#define UNUSED(x) (void)(x)

extern "C" int main(int argc, char *argv[], char *envp[])
{
    UNUSED(argc);
    UNUSED(argv);
    UNUSED(envp);

    // unsigned long ret;
    // asm volatile("syscall"
    //              : "=a"(ret)
    //              : "a"(1), "D"(1)
    //              : "rcx", "r11", "memory");

    // syscall_exit();
    return 0;
}
