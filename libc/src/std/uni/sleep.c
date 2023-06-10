#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include "../../../Kernel/syscalls.h"

unsigned int sleep(unsigned int seconds)
{
	return syscall1(_Sleep, seconds * 1000000);
}

int usleep(useconds_t usec)
{
	return syscall1(_Sleep, usec);
}
