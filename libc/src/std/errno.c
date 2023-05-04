#include <errno.h>

int __local_stub_errno = 0;

int *__errno_location(void)
{
    return &__local_stub_errno;
}
