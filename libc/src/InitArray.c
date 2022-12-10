// extern void (*__preinit_array_start[])(void) __attribute__((weak));
// extern void (*__preinit_array_end[])(void) __attribute__((weak));
// extern void (*__init_array_start[])(void) __attribute__((weak));
// extern void (*__init_array_end[])(void) __attribute__((weak));
// extern void (*__fini_array_start []) (void) __attribute__((weak));
// extern void (*__fini_array_end []) (void) __attribute__((weak));

typedef void (*CallPtr)(void);
extern CallPtr __init_array_start[0], __init_array_end[0];
extern CallPtr __fini_array_start[0], __fini_array_end[0];

// TODO: This is not working properly

void __libc_init_array(void)
{
    // for (CallPtr *func = __init_array_start; func != __init_array_end; func++)
    //     (*func)();
}

void __libc_fini_array(void)
{
    // for (CallPtr *func = __fini_array_start; func != __fini_array_end; func++)
    //     (*func)();
}
