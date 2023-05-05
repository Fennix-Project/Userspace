#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>
#include <stddef.h>
#include <sys/stat.h>

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

struct _IO_FILE
{
    size_t offset;

    void *KernelPrivate;
};

typedef struct _IO_FILE FILE;

#ifdef __cplusplus
extern "C"
{
#endif

    extern FILE *stdin;
    extern FILE *stdout;
    extern FILE *stderr;
    #define stdin stdin
    #define stdout stdout
    #define stderr stderr

    FILE *fopen(const char *filename, const char *mode);
    size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
    size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
    off_t fseek(FILE *stream, long offset, int whence);
    long ftell(FILE *stream);
    int fclose(FILE *fp);
    int fflush(FILE *stream);
    int fprintf(FILE *stream, const char *format, ...);
    int printf(const char *format, ...);
    void setbuf(FILE *stream, char *buf);
    int vfprintf(FILE *stream, const char *format, va_list arg);
    int vsscanf(const char *s, const char *format, va_list arg);
    int sscanf(const char *s, const char *format, ...);

    int fputc(int c, FILE *stream);
    int putc(int c, FILE *stream);
    int fputs(const char *s, FILE *stream);
    int puts(const char *s);
    int putchar(int c);

#ifdef __cplusplus
}
#endif

#endif
