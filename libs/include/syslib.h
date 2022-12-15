#ifndef __FENNIX_LIBS_SYS_H__
#define __FENNIX_LIBS_SYS_H__

#include <types.h>

enum KCtl
{
    KCTL_NULL,
    KCTL_GETPID,
    KCTL_GETTID,
    KCTL_GETUID,
    KCTL_GETGID,
};

long DoCtl(uint64_t Command, uint64_t Arg1, uint64_t Arg2, uint64_t Arg3, uint64_t Arg4);

#endif // !__FENNIX_LIBS_SYS_H__
