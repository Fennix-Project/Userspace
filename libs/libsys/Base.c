#include <sysbase.h>

#include "../../../Kernel/syscalls.h"

long DoCtl(uint64_t Command, uint64_t Arg1, uint64_t Arg2, uint64_t Arg3, uint64_t Arg4)
{
    return syscall5(_KernelCTL, Command, Arg1, Arg2, Arg3, Arg4);
}

uintptr_t KrnlRequestPages(size_t Count)
{
    return syscall1(_RequestPages, Count);
}

void KrnlFreePages(uintptr_t Address, size_t Count)
{
    syscall2(_FreePages, Address, Count);
}
