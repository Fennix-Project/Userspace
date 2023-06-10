#include <libsys/base.h>
#include <libsys/file.h>

#include "../../../Kernel/syscalls.h"

__attribute__((visibility("hidden"))) long __FILE_GetPageSize()
{
	static long PageSize = 0;
	if (PageSize == 0)
		PageSize = DoCtl(KCTL_GET_PAGE_SIZE, 0, 0, 0, 0);
	return PageSize;
}

File *FileOpen(const char *Path, uint64_t Flags)
{
	File *FilePtr = (File *)KrnlRequestPages(sizeof(File) / __FILE_GetPageSize() + 1);
	FilePtr->KernelPrivate = (void *)syscall2(_FileOpen, (uint64_t)Path, Flags);
	return FilePtr;
}

void FileClose(File *File)
{
	syscall1(_FileClose, (uint64_t)File->KernelPrivate);
	KrnlFreePages((uintptr_t)File, sizeof(File) / __FILE_GetPageSize() + 1);
}

uint64_t FileRead(File *File, uint64_t Offset, uint8_t *Buffer, uint64_t Size)
{
	return syscall4(_FileRead, (uint64_t)File->KernelPrivate, Offset, (uint64_t)Buffer, Size);
}

uint64_t FileWrite(File *File, uint64_t Offset, uint8_t *Buffer, uint64_t Size)
{
	return syscall4(_FileWrite, (uint64_t)File->KernelPrivate, Offset, (uint64_t)Buffer, Size);
}

uint64_t FileSeek(File *File, uint64_t Offset, uint64_t Whence)
{
	return syscall3(_FileSeek, (uint64_t)File->KernelPrivate, Offset, Whence);
}

uint64_t FileStatus(File *File)
{
	return syscall1(_FileStatus, (uint64_t)File->KernelPrivate);
}
