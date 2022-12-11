#include <stdio.h>
#include <stdarg.h>

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

// int vfprintf(FILE *stream, const char *format, va_list arg)
// {
//     return 0;
// }

int puts(const char *s)
{
    return 0;
}

int putchar(int c)
{
    return 0;
}
