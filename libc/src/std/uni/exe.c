#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include "../../../Kernel/syscalls.h"

int execl(const char *pathname, const char *arg, ...)
{
    errno = ENOSYS;
    return -1;
}

int execlp(const char *file, const char *arg, ...)
{
    errno = ENOSYS;
    return -1;
}

int execle(const char *pathname, const char *arg, ...)
{
    errno = ENOSYS;
    return -1;
}

int execv(const char *pathname, char *const argv[])
{
    errno = ENOSYS;
    return -1;
}

int execvp(const char *file, char *const argv[])
{
    errno = ENOSYS;
    return -1;
}

int execvpe(const char *file, char *const argv[], char *const envp[])
{
    errno = ENOSYS;
    return -1;
}

int execve(const char *pathname, char *const argv[], char *const envp[])
{
    errno = ENOSYS;
    return -1;
}

pid_t fork(void)
{
    return syscall0(_Fork);
}
