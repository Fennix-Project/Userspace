#include <stdio.h>
#include <stdarg.h>
#include <sys/syscalls.h>

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
