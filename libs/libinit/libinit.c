#include <libinit/init.h>
#include <unistd.h>
#include "printf.h"

void init_log(const char *fmt, ...)
{
	static short log_lock = 0;
	while (log_lock)
		usleep(1);
	__sync_synchronize();
	log_lock = 1;

	printf_libinit("\eCCCCCC[\e0088FFinit\eCCCCCC] \eAAAAAA");
	va_list args;
	va_start(args, fmt);
	vprintf_libinit(fmt, args);
	va_end(args);
	printf_libinit("\eCCCCCC");

	log_lock = 0;
	__sync_synchronize();
}
