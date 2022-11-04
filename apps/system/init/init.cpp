#include "../../../../Kernel/Fex.hpp"

extern "C" int _start(void *Data);

HEAD(FexFormatType_Executable, FexOSType_Fennix, _start);

#define DEFINE_SYSCALL0(function, n)          \
    static inline long syscall_##function()   \
    {                                         \
        long a = n;                           \
        __asm__ __volatile__("pushq %%r11\n"  \
                             "pushq %%rcx\n"  \
                             "syscall\n"      \
                             "popq %%rcx\n"   \
                             "popq %%r11\n"   \
                             : "=a"(a)        \
                             : "a"((long)a)); \
        return a;                             \
    }

DEFINE_SYSCALL0(exit, 1);
#define UNUSED(x) (void)(x)

extern "C" int _start(void *Data)
{
    UNUSED(Data);
    syscall_exit();
    return 0;
}
