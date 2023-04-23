#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void abort(void);
    int atexit(void (*function)(void));
    void exit(int status);
    int atoi(const char *nptr);
    char *getenv(const char *name);

    void *malloc(size_t Size);
    void *realloc(void *Address, size_t Size);
    void *calloc(size_t Count, size_t Size);
    void free(void *Address);
    int system(const char *command);

    double atof(const char *nptr);

#ifdef __cplusplus
}
#endif

#endif
