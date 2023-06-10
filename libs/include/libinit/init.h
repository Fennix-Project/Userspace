#ifndef __FENNIX_LIBS_INIT_H__
#define __FENNIX_LIBS_INIT_H__

#include <stddef.h>
#include <stdarg.h>

int printf_libinit(const char *format, ...);
int vprintf_libinit(const char *format, va_list arg);
int sprintf_libinit(char *s, const char *format, ...);
int vsprintf_libinit(char *s, const char *format, va_list arg);
int snprintf_libinit(char *s, size_t count, const char *format, ...);
int vsnprintf_libinit(char *s, size_t count, const char *format, va_list arg);

__attribute__((visibility("default"))) void init_log(const char *fmt, ...);

#endif // !__FENNIX_LIBS_INIT_H__
