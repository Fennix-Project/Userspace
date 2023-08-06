#include <libinit/init.h>
#include <unistd.h>
#include <features.h>
#include "printf.h"

#ifdef __FENNIX_LIBC__
#define cprintf printf_libinit
#define cvprintf vprintf_libinit
#else
#define cprintf printf
#define cvprintf vprintf
#endif

void init_log(const char *fmt, ...)
{
	static short log_lock = 0;
	while (log_lock)
		usleep(1);
	__sync_synchronize();
	log_lock = 1;

	cprintf("\eCCCCCC[\e0088FFinit\eCCCCCC] \eAAAAAA");
	va_list args;
	va_start(args, fmt);
	cvprintf(fmt, args);
	va_end(args);
	cprintf("\eCCCCCC");

	log_lock = 0;
	__sync_synchronize();
}
