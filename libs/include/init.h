#ifndef __FENNIX_LIBS_INIT_H__
#define __FENNIX_LIBS_INIT_H__

#include <types.h>

__attribute__((visibility("hidden"))) int printf_libinit(const char *format, ...);
__attribute__((visibility("hidden"))) int vprintf_libinit(const char *format, va_list arg);
__attribute__((visibility("hidden"))) int sprintf_libinit(char *s, const char *format, ...);
__attribute__((visibility("hidden"))) int vsprintf_libinit(char *s, const char *format, va_list arg);
__attribute__((visibility("hidden"))) int snprintf_libinit(char *s, size_t count, const char *format, ...);
__attribute__((visibility("hidden"))) int vsnprintf_libinit(char *s, size_t count, const char *format, va_list arg);

void init_log(const char *fmt, ...);

#endif // !__FENNIX_LIBS_INIT_H__
