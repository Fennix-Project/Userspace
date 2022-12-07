extern void (*__preinit_array_start[])(void) __attribute__((weak));
extern void (*__preinit_array_end[])(void) __attribute__((weak));
extern void (*__init_array_start[])(void) __attribute__((weak));
extern void (*__init_array_end[])(void) __attribute__((weak));
extern void (*__fini_array_start []) (void) __attribute__((weak));
extern void (*__fini_array_end []) (void) __attribute__((weak));

extern void _init(void);

void __libc_init_array(void)
{
    unsigned long Count = __preinit_array_end - __preinit_array_start;
    for (unsigned long i = 0; i < Count; i++)
        __preinit_array_start[i]();

    _init();

    Count = __init_array_end - __init_array_start;
    for (unsigned long i = 0; i < Count; i++)
        __init_array_start[i]();
}
