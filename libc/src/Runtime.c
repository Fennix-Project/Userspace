
extern void __libc_init_array(void);
extern void __libc_fini_array(void);

extern void __libc_init_std(void);
extern void __libc_fini_std(void);

void __libc_init(void)
{
    __libc_init_array();
    __libc_init_std();
}

void _exit(int Code)
{
    __libc_fini_std();
    __libc_fini_array();
    __asm__ __volatile__("syscall"
                         :
                         : "a"(0), "D"(Code)
                         : "rcx", "r11", "memory");

    while (1)
        ;
}
