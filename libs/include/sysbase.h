#ifndef __FENNIX_LIBS_BASE_H__
#define __FENNIX_LIBS_BASE_H__

#include <stddef.h>

enum KCtl
{
    KCTL_NULL,
    KCTL_GET_PID,
    KCTL_GET_TID,
    KCTL_GET_UID,
    KCTL_GET_GID,
    KCTL_GET_PAGE_SIZE,
    KCTL_IS_CRITICAL,
};

long DoCtl(uint64_t Command, uint64_t Arg1, uint64_t Arg2, uint64_t Arg3, uint64_t Arg4);

uintptr_t KrnlRequestPages(size_t Count);
void KrnlFreePages(uintptr_t Address, size_t Count);

#endif // !__FENNIX_LIBS_BASE_H__
