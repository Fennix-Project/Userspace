#ifndef __FENNIX_LIBS_BASE_H__
#define __FENNIX_LIBS_BASE_H__

#include <stddef.h>

long DoCtl(uint64_t Command, uint64_t Arg1, uint64_t Arg2, uint64_t Arg3, uint64_t Arg4);

uintptr_t KrnlRequestPages(size_t Count);
void KrnlFreePages(uintptr_t Address, size_t Count);

#endif // !__FENNIX_LIBS_BASE_H__
