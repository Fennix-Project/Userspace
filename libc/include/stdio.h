#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>
#include <stddef.h>

#define SEEK_SET 0

typedef struct
{
    int unused;
} FILE;

#ifdef __cplusplus
extern "C"
{
#endif

    extern FILE *stdin;
    extern FILE *stdout;
    extern FILE *stderr;

    int fclose(FILE *stream);
    int fflush(FILE *stream);
    FILE *fopen(const char *filename, const char *mode);
    int fprintf(FILE *stream, const char *format, ...);
    int printf(const char *format, ...);
    size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
    int fseek(FILE *stream, long offset, int whence);
    long ftell(FILE *stream);
    size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
    void setbuf(FILE *stream, char *buf);
    int vfprintf(FILE *stream, const char *format, va_list arg);

    int puts(const char *s);
    int putchar(int c);

#ifdef __cplusplus
}
#endif

#endif