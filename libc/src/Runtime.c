
extern void __libc_init_array();

void __libc_init()
{
    __libc_init_array();
}

void _exit(int Code)
{
    __asm__ __volatile__("syscall"
                         :
                         : "a"(0), "D"(Code)
                         : "rcx", "r11", "memory");

    while (1)
        __asm__ __volatile__("hlt");
}
