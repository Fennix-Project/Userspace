#ifndef __FENNIX_LIBS_INIT_H__
#define __FENNIX_LIBS_INIT_H__

#include <types.h>

int printf_libinit(const char *format, ...) __attribute__((format(__printf__, (1), (2))));
int vprintf_libinit(const char *format, va_list arg) __attribute__((format(__printf__, ((1)), (0))));
int sprintf_libinit(char *s, const char *format, ...) __attribute__((format(__printf__, (2), (3))));
int vsprintf_libinit(char *s, const char *format, va_list arg) __attribute__((format(__printf__, ((2)), (0))));
int snprintf_libinit(char *s, size_t count, const char *format, ...) __attribute__((format(__printf__, (3), (4))));
int vsnprintf_libinit(char *s, size_t count, const char *format, va_list arg) __attribute__((format(__printf__, ((3)), (0))));

void init_log(const char *fmt, ...) __attribute__((format(__printf__, (1), (2))));

#endif // !__FENNIX_LIBS_INIT_H__
