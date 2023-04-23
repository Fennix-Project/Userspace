#include <stdio.h>
#include <stdarg.h>
#include <sys/syscalls.h>

FILE *stdin;
FILE *stdout;
FILE *stderr;

int fclose(FILE *stream)
{
    return 0;
}

int fflush(FILE *stream)
{
    return 0;
}

FILE *fopen(const char *filename, const char *mode)
{
    return 0;
}

int fprintf(FILE *stream, const char *format, ...)
{
    return 0; // sprintf(char *s, const char *format, ...)
}

// int printf(const char *format, ...)
// {
//     return 0;
// }

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return 0;
}

int fseek(FILE *stream, long offset, int whence)
{
    return 0;
}

long ftell(FILE *stream)
{
    return 0;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return 0;
}

void setbuf(FILE *stream, char *buf)
{
}

int vfprintf(FILE *stream, const char *format, va_list arg)
{
    return 0;
}

int vsscanf(const char *s, const char *format, va_list arg)
{
}

int sscanf(const char *s, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    const int ret = vsscanf(s, format, args);
    va_end(args);
    return ret;
}

int fputc(int c, FILE *stream)
{
    syscall1(_Print, c);
}

int putc(int c, FILE *stream)
{
    syscall1(_Print, c);
}

int fputs(const char *s, FILE *stream)
{
    for (int i = 0; s[i] != '\0'; i++)
        fputc(s[i], stream);
}

int putchar(int c)
{
    syscall1(_Print, c);
}

int puts(const char *s)
{
    for (int i = 0; s[i] != '\0'; i++)
        fputc(s[i], stdout);
}
