#include "ld.h"
#include "fcts.h"

#include "../../../Kernel/syscalls.h"
#include "../../../Kernel/ipc.h"
#include "elf.h"

#define Print(x) Print__(x)
#define PrintNL(x) PrintNL__(x)

#if (1)
#define PrintDbg(x) Print__(x)
#define PrintDbgNL(x) PrintNL__(x)
#define ltoaDbg(x, y, z) ltoa(x, y, z)
#else
#define PrintDbg(x)
#define PrintDbgNL(x)
#define ltoaDbg(x, y, z)
#endif

struct InterpreterIPCDataLibrary
{
	char Name[64];
};

typedef struct
{
	char Path[256];
	void *MemoryImage;
	struct InterpreterIPCDataLibrary Libraries[64];
} InterpreterIPCData;

struct LibsCollection
{
	char ParentName[32];
	char LibraryName[32];
	uintptr_t ParentMemoryImage;
	uintptr_t LibraryMemoryImage;
	struct LibsCollection *Next;
	char Valid;
};

static char ParentPath[256];
static char Lock = 0;

__attribute__((naked, used, no_stack_protector)) void ELF_LAZY_RESOLVE_STUB()
{
	while (Lock == 1)
		;
	__asm__ __volatile__("mfence\n");
	Lock = 1;
	__asm__ __volatile__("pop %r11\n"
						 "pop %r10\n"

						 "push %rdi\n"
						 "push %rsi\n"
						 "push %rdx\n"
						 "push %rcx\n"
						 "push %r8\n"
						 "push %r9\n"

						 "mov %r11, %rdi\n" // Move the first argument to rdi (libs collection)
						 "mov %r10, %rsi\n" // Move the second argument to rsi (rel index)

						 "call ELF_LAZY_RESOLVE_MAIN\n"

						 "mov %rax, %r11\n" // Move the return value to r11

						 "pop %r9\n"
						 "pop %r8\n"
						 "pop %rcx\n"
						 "pop %rdx\n"
						 "pop %rsi\n"
						 "pop %rdi\n"

						 "jmp *%r11\n"); // Jump to the return value
}

void (*ELF_LAZY_RESOLVE_MAIN(struct LibsCollection *Info, long RelIndex))()
{
	if (!Info)
		goto FailEnd;

	char DbgBuff[32];
	char LibraryPathBuffer[256];

	struct LibsCollection *CurLib = Info;
	PrintDbgNL("_______");
	/* The last entry is the null entry (Valid == false)
		which determines the end of the list. */
	while (CurLib->Valid)
	{
		KernelCTL(KCTL_GET_ABSOLUTE_PATH, CurLib->LibraryName,
				  LibraryPathBuffer, sizeof(LibraryPathBuffer), 0);

		PrintDbg("-- ");
		PrintDbg(LibraryPathBuffer);
		PrintDbg(" ");
		ltoaDbg(RelIndex, DbgBuff, 10);
		PrintDbg(DbgBuff);
		PrintDbgNL(" --");
		uintptr_t lib_BaseAddress = __UINTPTR_MAX__;
		uintptr_t app_BaseAddress = __UINTPTR_MAX__;

		Elf64_Ehdr lib_Header;
		Elf64_Ehdr app_Header;

		void *KP_lib = syscall2(_FileOpen, LibraryPathBuffer, "r");
		void *KP_app = syscall2(_FileOpen, ParentPath, "r");

		if (!KP_lib)
		{
			PrintNL("Failed to open library");
			goto RetryNextLib;
		}

		if (!KP_app)
		{
			PrintNL("Failed to open application");
			goto RetryNextLib;
		}

		syscall3(_FileRead, KP_lib, &lib_Header, sizeof(Elf64_Ehdr));
		syscall3(_FileRead, KP_app, &app_Header, sizeof(Elf64_Ehdr));

		Elf64_Phdr ItrProgramHeader;

		for (Elf64_Half i = 0; i < lib_Header.e_phnum; i++)
		{
			syscall3(_FileSeek, KP_lib,
					 lib_Header.e_phoff +
						 lib_Header.e_phentsize * i,
					 SEEK_SET);

			syscall3(_FileRead, KP_lib, &ItrProgramHeader, sizeof(Elf64_Phdr));

			lib_BaseAddress = MIN(lib_BaseAddress, ItrProgramHeader.p_vaddr);
		}

		for (Elf64_Half i = 0; i < app_Header.e_phnum; i++)
		{
			syscall3(_FileSeek, KP_app,
					 app_Header.e_phoff +
						 app_Header.e_phentsize * i,
					 SEEK_SET);

			syscall3(_FileRead, KP_app, &ItrProgramHeader, sizeof(Elf64_Phdr));

			app_BaseAddress = MIN(app_BaseAddress, ItrProgramHeader.p_vaddr);
		}

		struct Elf64_Dyn lib_JmpRel = ELFGetDynamicTag(LibraryPathBuffer, DT_JMPREL);
		struct Elf64_Dyn lib_SymTab = ELFGetDynamicTag(LibraryPathBuffer, DT_SYMTAB);
		struct Elf64_Dyn lib_StrTab = ELFGetDynamicTag(LibraryPathBuffer, DT_STRTAB);

		struct Elf64_Dyn app_JmpRel = ELFGetDynamicTag(ParentPath, DT_JMPREL);
		struct Elf64_Dyn app_SymTab = ELFGetDynamicTag(ParentPath, DT_SYMTAB);
		struct Elf64_Dyn app_StrTab = ELFGetDynamicTag(ParentPath, DT_STRTAB);

		if (!lib_JmpRel.d_tag == 0)
		{
			PrintNL("No DT_JMPREL");
			// goto RetryNextLib;
		}
		else if (RelIndex >= lib_JmpRel.d_un.d_val /
								 sizeof(Elf64_Rela))
		{
			PrintNL("RelIndex is greater than the number of relocations");
			goto RetryNextLib;
		}

		if (!lib_SymTab.d_tag == 0)
		{
			PrintNL("No DT_SYMTAB");
			goto RetryNextLib;
		}

		if (!lib_StrTab.d_tag == 0)
		{
			PrintNL("No DT_STRTAB");
			goto RetryNextLib;
		}

		if (!lib_SymTab.d_tag == 0 &&
			!lib_StrTab.d_tag == 0)
			goto RetryNextLib;

		Elf64_Rela *_lib_JmpRel = (Elf64_Rela *)(CurLib->LibraryMemoryImage + (lib_JmpRel.d_un.d_ptr - lib_BaseAddress));
		Elf64_Sym *_lib_SymTab = (Elf64_Sym *)(CurLib->LibraryMemoryImage + (lib_SymTab.d_un.d_ptr - lib_BaseAddress));

		Elf64_Rela *_app_JmpRel = (Elf64_Rela *)(CurLib->ParentMemoryImage + (app_JmpRel.d_un.d_ptr - app_BaseAddress));
		Elf64_Sym *_app_SymTab = (Elf64_Sym *)(CurLib->ParentMemoryImage + (app_SymTab.d_un.d_ptr - app_BaseAddress));

		char *lib_DynStr = (char *)(CurLib->LibraryMemoryImage + (lib_StrTab.d_un.d_ptr - lib_BaseAddress));
		char *app_DynStr = (char *)(CurLib->ParentMemoryImage + (app_StrTab.d_un.d_ptr - app_BaseAddress));

		Elf64_Rela *Rel = _app_JmpRel + RelIndex;
		Elf64_Addr *GOTEntry = (Elf64_Addr *)(Rel->r_offset);

		int RelType = ELF64_R_TYPE(Rel->r_info);

		switch (RelType)
		{
		case R_X86_64_NONE:
		{
			PrintDbgNL("R_X86_64_NONE");
			if (*GOTEntry == 0)
			{
				PrintDbgNL("GOTEntry is 0");
				break;
			}
			Lock = 0;
			return (void (*)()) * GOTEntry;
		}
		case R_X86_64_JUMP_SLOT:
		{
			PrintDbgNL("R_X86_64_JUMP_SLOT");
			int SymIndex = ELF64_R_SYM(Rel->r_info);
			Elf64_Sym *Sym = _app_SymTab + SymIndex;

			if (Sym->st_name)
			{
				char *SymName = app_DynStr + Sym->st_name;
				PrintDbg("SymName: ");
				PrintDbgNL(SymName);

				Elf64_Sym LibSym = ELFLookupSymbol(ParentPath, SymName);

				PrintDbg("LibSym: 0x");
				ltoaDbg((long)LibSym.st_size, DbgBuff, 16);
				PrintDbgNL(DbgBuff);

				if (LibSym.st_value)
				{
					*GOTEntry = (Elf64_Addr)(CurLib->LibraryMemoryImage + LibSym.st_value);

					ltoa(*GOTEntry, DbgBuff, 16);
					PrintDbg("*GOTEntry: 0x");
					PrintDbgNL(DbgBuff);

					Lock = 0;
					return (void (*)()) * GOTEntry;
				}
				PrintDbgNL("Not found in lib");
			}
			break;
		}
		default:
		{
			ltoa(RelType, DbgBuff, 10);
			Print("RelType not supported ");
			PrintNL(DbgBuff);
			break;
		}
		}

	RetryNextLib:
		PrintDbgNL("Retrying next lib");
		CurLib = CurLib->Next;
	}

FailEnd:
	Lock = 0;
	__asm__ __volatile__("mfence\n");

	Print("Symbol index ");
	ltoa(RelIndex, DbgBuff, 10);
	Print(DbgBuff);
	PrintNL(" not found");
	int ExitCode = 0x51801;
	syscall1(_Exit, ExitCode);
	while (1) // Make sure we don't return
		;
}

/* Preload */
int ld_main()
{
	/* Prevent race condition. */
	uintptr_t KCTL_ret = KernelCTL(KCTL_IS_CRITICAL, 0, 0, 0, 0);
	do
	{
		syscall1(_Sleep, 250);
		KCTL_ret = KernelCTL(KCTL_IS_CRITICAL, 0, 0, 0, 0);
	} while (KCTL_ret == SYSCALL_ACCESS_DENIED);

	if (KCTL_ret == false)
		return -1;

	/* Everything is ok, continue. */
	return 0;
}

bool ELFAddLazyResolverToGOT(void *MemoryImage, struct LibsCollection *Libs)
{
	struct Elf64_Dyn Dyn = ELFGetDynamicTag(ParentPath, DT_PLTGOT);
	if (!Dyn.d_tag)
		return false;

	Elf64_Addr *GOT = (Elf64_Addr *)Dyn.d_un.d_ptr;

	GOT[1] = (uintptr_t)Libs;
	GOT[2] = (uintptr_t)ELF_LAZY_RESOLVE_STUB;
	return true;
}

/* Actual load */
int ld_load(int argc, char *argv[], char *envp[])
{
	PrintDbgNL("!");
	uintptr_t PageSize = KernelCTL(KCTL_GET_PAGE_SIZE, 0, 0, 0, 0);
	int PagesForIPCDataStruct = sizeof(InterpreterIPCData) / PageSize + 1;
	int PagesForLibsCollectionStruct = sizeof(struct LibsCollection) / PageSize + 1;

	InterpreterIPCData *IPCBuffer =
		(InterpreterIPCData *)RequestPages(PagesForIPCDataStruct);

	int IPC_ID = IPC(IPC_CREATE, IPC_TYPE_MessagePassing,
					 0, 0, "LOAD", sizeof(InterpreterIPCData));
	while (true)
	{
		IPC(IPC_LISTEN, IPC_TYPE_MessagePassing, IPC_ID, 1, NULL, 0);
		IPC(IPC_WAIT, IPC_TYPE_MessagePassing, IPC_ID, 0, NULL, 0);
		int IPCResult = IPC(IPC_READ, IPC_TYPE_MessagePassing,
							IPC_ID, 0, IPCBuffer, PageSize);

		if (IPCResult == IPC_E_CODE_Success)
			break;
	}

	struct LibsCollection *LibsForLazyResolver =
		(struct LibsCollection *)RequestPages(PagesForLibsCollectionStruct);

	for (short i = 0; i < 64; i++)
	{
		if (IPCBuffer->Libraries[i].Name[0] == '\0')
			break;

		uintptr_t lib_mm_image =
			KernelCTL(KCTL_GET_ELF_LIB_MEMORY_IMAGE,
					  (uint64_t)IPCBuffer->Libraries[i].Name, 0, 0, 0);
		if (lib_mm_image == 0)
		{
			enum SyscallsErrorCodes ret =
				KernelCTL(KCTL_REGISTER_ELF_LIB,
						  (uint64_t)IPCBuffer->Libraries[i].Name,
						  (uint64_t)IPCBuffer->Libraries[i].Name, 0, 0);
			if (ret != SYSCALL_OK)
			{
				PrintNL("Failed to register ELF lib");
				return -0x11B;
			}
			lib_mm_image = KernelCTL(KCTL_GET_ELF_LIB_MEMORY_IMAGE,
									 (uint64_t)IPCBuffer->Libraries[i].Name, 0, 0, 0);
		}

		if (LibsForLazyResolver->Next == NULL)
		{
			LibsForLazyResolver->Valid = true;
			LibsForLazyResolver->LibraryMemoryImage = (uintptr_t)lib_mm_image;
			LibsForLazyResolver->ParentMemoryImage = (uintptr_t)IPCBuffer->MemoryImage;
			for (short j = 0; j < sizeof(LibsForLazyResolver->LibraryName); j++)
				LibsForLazyResolver->LibraryName[j] = IPCBuffer->Libraries[i].Name[j];

			LibsForLazyResolver->Next =
				(struct LibsCollection *)RequestPages(PagesForLibsCollectionStruct);
			memset(LibsForLazyResolver->Next, 0, sizeof(struct LibsCollection));
			continue;
		}

		struct LibsCollection *CurrentLib = LibsForLazyResolver;
		while (CurrentLib->Next != NULL)
			CurrentLib = CurrentLib->Next;

		CurrentLib->Valid = true;
		CurrentLib->LibraryMemoryImage = (uintptr_t)lib_mm_image;
		CurrentLib->ParentMemoryImage = (uintptr_t)IPCBuffer->MemoryImage;
		for (short j = 0; j < sizeof(LibsForLazyResolver->LibraryName); j++)
			CurrentLib->LibraryName[j] = IPCBuffer->Libraries[i].Name[j];

		CurrentLib->Next =
			(struct LibsCollection *)RequestPages(PagesForLibsCollectionStruct);
		memset(CurrentLib->Next, 0, sizeof(struct LibsCollection));
	}

	struct LibsCollection *CurrentLib = LibsForLazyResolver;

	for (int i = 0; i < sizeof(ParentPath); i++)
		ParentPath[i] = IPCBuffer->Path[i];

	if (!ELFAddLazyResolverToGOT(IPCBuffer->MemoryImage,
								 LibsForLazyResolver))
	{
		PrintNL("Failed to add lazy resolver to GOT");
		return -0x607;
	}

	void *KP = syscall2(_FileOpen, ParentPath, (long)"r");
	if (KP == NULL)
	{
		PrintNL("Failed to open file");
		syscall1(_Exit, -0xF17E);
	}

	Elf64_Ehdr ELFHeader;
	syscall3(_FileRead, KP, &ELFHeader, sizeof(Elf64_Ehdr));

	Elf64_Addr Entry = ELFHeader.e_entry;

	syscall1(_FileClose, KP);

	IPC(IPC_DELETE, IPC_TYPE_MessagePassing, IPC_ID, 0, NULL, 0);
	FreePages((uintptr_t)IPCBuffer, PagesForIPCDataStruct);

	PrintDbgNL("Calling entry point");
	return ((int (*)(int, char *[], char *[]))Entry)(argc, argv, envp);
}
