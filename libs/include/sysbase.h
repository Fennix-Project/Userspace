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

    /**
     * @brief Get the page size
     */
    KCTL_GET_PAGE_SIZE,

    /**
     * @brief Check whether the current thread is critical
     */
    KCTL_IS_CRITICAL,

    /**
     * @brief Register an ELF library
     * @fn int RegisterELFLib(char *Identifier, char *Path)
     */
    KCTL_REGISTER_ELF_LIB,

    /**
     * @brief Get an ELF library
     * @fn uintptr_t GetELFLib(char *Identifier);
     */
    KCTL_GET_ELF_LIB_FILE,

    /**
     * @brief Get an ELF library
     * @fn uintptr_t GetELFLib(char *Identifier);
     */
    KCTL_GET_ELF_LIB_MEMORY_IMAGE,

    KCTL_GET_FRAMEBUFFER_BUFFER,
    KCTL_GET_FRAMEBUFFER_WIDTH,
    KCTL_GET_FRAMEBUFFER_HEIGHT,
    KCTL_GET_FRAMEBUFFER_SIZE,
};

long DoCtl(uint64_t Command, uint64_t Arg1, uint64_t Arg2, uint64_t Arg3, uint64_t Arg4);

uintptr_t KrnlRequestPages(size_t Count);
void KrnlFreePages(uintptr_t Address, size_t Count);

#endif // !__FENNIX_LIBS_BASE_H__
