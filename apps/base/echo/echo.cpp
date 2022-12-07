static inline long syscall2(int sc, long arg1, long arg2)
{
    long ret;
    __asm__ __volatile__("syscall"
                         : "=a"(ret)
                         : "a"(sc), "D"(arg1), "S"(arg2)
                         : "rcx", "r11", "memory");
    return ret;
}

int main(int argc, char *argv[])
{
    // TODO: Change this to use stdout
    for (int i = 1; i < argc; i++)
    {
        for (int j = 0; argv[i][j]; j++)
            syscall2(1, argv[i][j], 0);
        syscall2(1, ' ', 0);
    }
    return 0;
}
