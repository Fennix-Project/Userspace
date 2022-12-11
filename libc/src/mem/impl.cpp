#include <types.h>
#include "../lock.hpp"

#include "../../../../Kernel/syscalls.h"

NewLock(liballocLock);

extern "C" int liballoc_lock()
{
    return liballocLock.Lock(__FUNCTION__);
}

extern "C" int liballoc_unlock()
{
    return liballocLock.Unlock();
}

extern "C" void *liballoc_alloc(size_t Pages)
{
    return (void *)syscall1(_RequestPages, Pages);
}

extern "C" int liballoc_free(void *Address, size_t Pages)
{
    return syscall2(_FreePages, (uint64_t)Address, Pages);
}
