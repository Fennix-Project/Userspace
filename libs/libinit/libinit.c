#include <init.h>

void init_log(const char *fmt, ...)
{
    printf_libinit("\eCCCCCC[\e0088FFinit\eCCCCCC] \eAAAAAA");
    va_list args;
    va_start(args, fmt);
    vprintf_libinit(fmt, args);
    va_end(args);
}
