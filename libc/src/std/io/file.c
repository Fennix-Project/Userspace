#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/syscalls.h>

FILE *stdin;
FILE *stdout;
FILE *stderr;

FILE *fopen(const char *filename, const char *mode)
{
    FILE *FilePtr = malloc(sizeof(FILE));
    FilePtr->KernelPrivate = (void *)syscall2(_FileOpen, (uint64_t)filename, (uint64_t)mode);
    return FilePtr;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return syscall4(_FileRead, (uint64_t)stream->KernelPrivate, stream->offset, (uint64_t)ptr, size * nmemb);
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return syscall4(_FileWrite, (uint64_t)stream->KernelPrivate, stream->offset, (uint64_t)ptr, size * nmemb);
}

int fseek(FILE *stream, long offset, int whence)
{
    switch (whence)
    {
    case SEEK_SET:
        stream->offset = offset;
        break;
    case SEEK_CUR:
        stream->offset += offset;
        break;
    case SEEK_END:
        // stream->offset = syscall1(_FileLength, (uint64_t)File->KernelPrivate) + offset;
        break;
    default:
        return -1;
    }
    return stream->offset;
}

long ftell(FILE *stream)
{
    return stream->offset;
}

int fclose(FILE *fp)
{
    void *KP = fp->KernelPrivate;
    free(fp);
    return syscall1(_FileClose, (uint64_t)KP);
}

int fflush(FILE *stream)
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
