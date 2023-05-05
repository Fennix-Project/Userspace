#include <stdio.h>
#include <stdarg.h>
#include <sys/syscalls.h>

int fputc(int c, FILE *stream)
{
    return syscall2(_Print, c, 0);
}

int putc(int c, FILE *stream)
{
    return syscall2(_Print, c, 0);
}

int fputs(const char *s, FILE *stream)
{
    for (int i = 0; s[i] != '\0'; i++)
        fputc(s[i], stream);
}

int puts(const char *s)
{
    for (int i = 0; s[i] != '\0'; i++)
        fputc(s[i], stdout);
}

int putchar(int c)
{
    return putc(c, stdout);
}

void perror(const char *s)
{
    fputs(s, stderr);
}