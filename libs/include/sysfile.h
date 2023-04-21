#ifndef __FENNIX_LIBS_SYS_FILE_H__
#define __FENNIX_LIBS_SYS_FILE_H__

#include <stddef.h>

typedef struct
{
    void *KernelPrivate;
} File;

enum FileFlags
{
    FILE_READ = 1,
    FILE_WRITE = 2,
    FILE_APPEND = 4,
    FILE_CREATE = 8,
    FILE_TRUNCATE = 16,
    FILE_EXCLUSIVE = 32,
    FILE_DIRECTORY = 64,
    FILE_SYMLINK = 128,
    FILE_NONBLOCK = 256,
    FILE_CLOEXEC = 512,
};

File *FileOpen(const char *Path, uint64_t Flags);
void FileClose(File *File);
uint64_t FileRead(File *File, uint64_t Offset, uint8_t *Buffer, uint64_t Size);
uint64_t FileWrite(File *File, uint64_t Offset, uint8_t *Buffer, uint64_t Size);
uint64_t FileSeek(File *File, uint64_t Offset, uint64_t Whence);
uint64_t FileStatus(File *File);

#endif // !__FENNIX_LIBS_SYS_FILE_H__
