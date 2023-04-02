#include "ld.h"

#include "../../libs/include/sysbase.h"
#include "../../../Kernel/syscalls.h"
#include "../../../Kernel/ipc.h"
#include "elf.h"

uintptr_t RequestPages(size_t Count)
{
    return syscall1(_RequestPages, Count);
}

int FreePages(uintptr_t Address, size_t Count)
{
    return syscall2(_FreePages, Address, Count);
}

int IPC(enum IPCCommand Command, enum IPCType Type, int ID, int Flags, void *Buffer, size_t Size)
{
    return syscall6(_IPC, (long)Command, (long)Type, (long)ID, (long)Flags, (long)Buffer, (long)Size);
}

uintptr_t KernelCTL(enum KCtl Command, uint64_t Arg1, uint64_t Arg2, uint64_t Arg3, uint64_t Arg4)
{
    return syscall5(_KernelCTL, Command, Arg1, Arg2, Arg3, Arg4);
}

struct LibAddressCollection
{
    char Name[32];
    uintptr_t ElfFile;
    uintptr_t MemoryImage;
    uintptr_t ParentElfFile;
    uintptr_t ParentMemoryImage;
    struct LibAddressCollection *Next;
    char Valid;
};

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

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

int abs(int i) { return i < 0 ? -i : i; }

void swap(char *x, char *y)
{
    char t = *x;
    *x = *y;
    *y = t;
}

char *reverse(char *Buffer, int i, int j)
{
    while (i < j)
        swap(&Buffer[i++], &Buffer[j--]);
    return Buffer;
}

char *ltoa(long Value, char *Buffer, int Base)
{
    if (Base < 2 || Base > 32)
        return Buffer;

    long n = (long)abs((int)Value);
    int i = 0;

    while (n)
    {
        int r = n % Base;
        if (r >= 10)
            Buffer[i++] = 65 + (r - 10);
        else
            Buffer[i++] = 48 + r;
        n = n / Base;
    }

    if (i == 0)
        Buffer[i++] = '0';

    if (Value < 0 && Base == 10)
        Buffer[i++] = '-';

    Buffer[i] = '\0';
    return reverse(Buffer, 0, i - 1);
}

static inline void PutCharToKernelConsole(char c)
{
    __asm__ __volatile__("syscall"
                         :
                         : "a"(1), "D"(c), "S"(0)
                         : "rcx", "r11", "memory");
}

void Print(char *String)
{
    for (short i = 0; String[i] != '\0'; i++)
        PutCharToKernelConsole(String[i]);
}

void PrintNL(char *String)
{
    Print(String);
    Print("\n");
}

void *memcpy(void *dest, const void *src, __SIZE_TYPE__ n)
{
    uint8_t *d = dest;
    const uint8_t *s = src;
    while (n--)
        *d++ = *s++;
    return dest;
}

void *memset(void *s, int c, __SIZE_TYPE__ n)
{
    uint8_t *p = s;
    while (n--)
        *p++ = c;
}

int strcmp(const char *l, const char *r)
{
    for (; *l == *r && *l; l++, r++)
        ;
    return *(unsigned char *)l - *(unsigned char *)r;
}

struct Elf64_Dyn *ELFGetDynamicTag(void *ElfFile, enum DynamicArrayTags Tag)
{
    Elf64_Ehdr *ELFHeader = (Elf64_Ehdr *)ElfFile;

    Elf64_Phdr ItrProgramHeader;
    for (Elf64_Half i = 0; i < ELFHeader->e_phnum; i++)
    {
        memcpy(&ItrProgramHeader, (uint8_t *)ElfFile + ELFHeader->e_phoff + ELFHeader->e_phentsize * i, sizeof(Elf64_Phdr));
        if (ItrProgramHeader.p_type == PT_DYNAMIC)
        {
            struct Elf64_Dyn *Dynamic = (struct Elf64_Dyn *)((uint8_t *)ElfFile + ItrProgramHeader.p_offset);
            for (__SIZE_TYPE__ i = 0; i < ItrProgramHeader.p_filesz / sizeof(struct Elf64_Dyn); i++)
            {
                if (Dynamic[i].d_tag == Tag)
                {
                    // debug("Found dynamic tag %d at %#lx [d_val: %#lx].", Tag, &Dynamic[i], Dynamic[i].d_un.d_val);
                    return &Dynamic[i];
                }
                if (Dynamic[i].d_tag == DT_NULL)
                {
                    // debug("Reached end of dynamic tag list for tag %d.", Tag);
                    return (void *)0;
                }
            }
        }
    }
    // debug("Dynamic tag %d not found.", Tag);
    return (void *)0;
}

Elf64_Sym *ELFGetSymbol(uintptr_t ElfFile, char *SymbolName)
{
    struct Elf64_Dyn *symTab = ELFGetDynamicTag((void *)ElfFile, DT_SYMTAB);
    struct Elf64_Dyn *strTab = ELFGetDynamicTag((void *)ElfFile, DT_STRTAB);
    Elf64_Sym *DynSym = (Elf64_Sym *)(ElfFile + symTab->d_un.d_ptr);
    char *dynStr = (char *)(ElfFile + strTab->d_un.d_ptr);

    for (int i = 0; i < symTab->d_un.d_val; i++)
    {
        if (strcmp(dynStr + DynSym[i].st_name, SymbolName) == 0)
            return &DynSym[i];
    }
    PrintNL("ELFGetSymbol: Symbol not found!");
    return (Elf64_Sym *)0;
}

void (*ELF_LAZY_RESOLVE_MAIN(struct LibAddressCollection *Info, long RelIndex))()
{
    char DbgBuff[32];
    if (Info)
    {
        struct LibAddressCollection *CurLib = Info;
        PrintNL("_______");
        // The last entry is the null entry (Valid == false) which determines the end of the list.
        while (CurLib->Valid)
        {
            Print("-- ");
            Print(CurLib->Name);
            Print(" ");
            ltoa(RelIndex, DbgBuff, 10);
            Print(DbgBuff);
            PrintNL(" --");
            uintptr_t lib_BaseAddress = __UINTPTR_MAX__;
            uintptr_t app_BaseAddress = __UINTPTR_MAX__;

            Elf64_Phdr ItrProgramHeader;

            for (Elf64_Half i = 0; i < ((Elf64_Ehdr *)CurLib->ElfFile)->e_phnum; i++)
            {
                memcpy(&ItrProgramHeader,
                       (uint8_t *)CurLib->ElfFile +
                           ((Elf64_Ehdr *)CurLib->ElfFile)->e_phoff +
                           ((Elf64_Ehdr *)CurLib->ElfFile)->e_phentsize * i,
                       sizeof(Elf64_Phdr));

                lib_BaseAddress = MIN(lib_BaseAddress, ItrProgramHeader.p_vaddr);
            }

            for (Elf64_Half i = 0; i < ((Elf64_Ehdr *)CurLib->ParentElfFile)->e_phnum; i++)
            {
                memcpy(&ItrProgramHeader,
                       (uint8_t *)CurLib->ParentElfFile +
                           ((Elf64_Ehdr *)CurLib->ParentElfFile)->e_phoff +
                           ((Elf64_Ehdr *)CurLib->ParentElfFile)->e_phentsize * i,
                       sizeof(Elf64_Phdr));

                app_BaseAddress = MIN(app_BaseAddress, ItrProgramHeader.p_vaddr);
            }

            ltoa((long)CurLib->MemoryImage, DbgBuff, 16);
            Print("lib:mmImg 0x");
            PrintNL(DbgBuff);

            ltoa((long)CurLib->ParentMemoryImage, DbgBuff, 16);
            Print("lib:mmImg 0x");
            PrintNL(DbgBuff);

            ltoa(lib_BaseAddress, DbgBuff, 16);
            Print("lib:BAddr 0x");
            PrintNL(DbgBuff);

            ltoa(app_BaseAddress, DbgBuff, 16);
            Print("lib:BAddr 0x");
            PrintNL(DbgBuff);

            struct Elf64_Dyn *lib_JmpRel = ELFGetDynamicTag((void *)CurLib->ElfFile, DT_JMPREL);
            struct Elf64_Dyn *lib_SymTab = ELFGetDynamicTag((void *)CurLib->ElfFile, DT_SYMTAB);
            struct Elf64_Dyn *lib_StrTab = ELFGetDynamicTag((void *)CurLib->ElfFile, DT_STRTAB);

            struct Elf64_Dyn *app_JmpRel = ELFGetDynamicTag((void *)CurLib->ParentElfFile, DT_JMPREL);
            struct Elf64_Dyn *app_SymTab = ELFGetDynamicTag((void *)CurLib->ParentElfFile, DT_SYMTAB);
            struct Elf64_Dyn *app_StrTab = ELFGetDynamicTag((void *)CurLib->ParentElfFile, DT_STRTAB);

            if (!lib_JmpRel)
            {
                PrintNL("No DT_JMPREL");
                goto RetryNextLib;
            }
            else if (RelIndex >= lib_JmpRel->d_un.d_val / sizeof(Elf64_Rela))
            {
                PrintNL("RelIndex is greater than the number of relocations");
                goto RetryNextLib;
            }

            if (!lib_SymTab)
            {
                PrintNL("No DT_SYMTAB");
                goto RetryNextLib;
            }

            if (!lib_StrTab)
            {
                PrintNL("No DT_STRTAB");
                goto RetryNextLib;
            }

            if (!lib_JmpRel && !lib_SymTab && !lib_StrTab)
                goto RetryNextLib;

            Elf64_Rela *_lib_JmpRel = (Elf64_Rela *)(CurLib->MemoryImage + (lib_JmpRel->d_un.d_ptr - lib_BaseAddress));
            Elf64_Sym *_lib_SymTab = (Elf64_Sym *)(CurLib->MemoryImage + (lib_SymTab->d_un.d_ptr - lib_BaseAddress));

            Elf64_Rela *_app_JmpRel = (Elf64_Rela *)(CurLib->ParentMemoryImage + (app_JmpRel->d_un.d_ptr - app_BaseAddress));
            Elf64_Sym *_app_SymTab = (Elf64_Sym *)(CurLib->ParentMemoryImage + (app_SymTab->d_un.d_ptr - app_BaseAddress));

            char *lib_DynStr = (char *)(CurLib->MemoryImage + (lib_StrTab->d_un.d_ptr - lib_BaseAddress));
            char *app_DynStr = (char *)(CurLib->ParentMemoryImage + (app_StrTab->d_un.d_ptr - app_BaseAddress));

            Elf64_Rela *Rel = _app_JmpRel + RelIndex;
            Elf64_Addr *GOTEntry = (Elf64_Addr *)(Rel->r_offset);

            int RelType = ELF64_R_TYPE(Rel->r_info);

            switch (RelType)
            {
            case R_X86_64_NONE:
            {
                PrintNL("R_X86_64_NONE");
                if (*GOTEntry == 0)
                {
                    PrintNL("GOTEntry is 0");
                    break;
                }
                Lock = 0;
                return (void (*)()) * GOTEntry;
            }
            case R_X86_64_JUMP_SLOT:
            {
                PrintNL("R_X86_64_JUMP_SLOT");
                int SymIndex = ELF64_R_SYM(Rel->r_info);
                Elf64_Sym *Sym = _app_SymTab + SymIndex;

                if (Sym->st_name)
                {
                    char *SymName = app_DynStr + Sym->st_name;
                    Print("SymName: ");
                    PrintNL(SymName);

                    Elf64_Sym *LibSym = ELFGetSymbol(CurLib->ElfFile, SymName);
                    if (LibSym)
                    {
                        *GOTEntry = (Elf64_Addr)(CurLib->MemoryImage + LibSym->st_value);

                        ltoa(*GOTEntry, DbgBuff, 16);
                        Print("*GOTEntry: 0x");
                        PrintNL(DbgBuff);

                        Lock = 0;
                        return (void (*)()) * GOTEntry;
                    }
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
            PrintNL("Retrying next lib");
            CurLib = CurLib->Next;
        }
    }

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

struct InterpreterIPCDataLibrary
{
    char Name[128];
};

typedef struct
{
    char Path[256];
    void *ElfFile;
    void *MemoryImage;
    struct InterpreterIPCDataLibrary Libraries[64];
} InterpreterIPCData;

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
        return -4;

    /* Everything is ok, continue. */
    return 0;
}

bool ELFAddLazyResolverToGOT(void *ElfFile, void *MemoryImage, struct LibAddressCollection *Libs)
{
    struct Elf64_Dyn *Dyn = (struct Elf64_Dyn *)ELFGetDynamicTag(ElfFile, DT_PLTGOT);
    if (!Dyn)
        return false;

    Elf64_Addr *GOT = (Elf64_Addr *)Dyn->d_un.d_ptr;

    GOT[1] = (uintptr_t)Libs;
    GOT[2] = (uintptr_t)ELF_LAZY_RESOLVE_STUB;
    return true;
}

/* Actual load */
int ld_load(int argc, char *argv[], char *envp[])
{
    uintptr_t PageSize = KernelCTL(KCTL_GET_PAGE_SIZE, 0, 0, 0, 0);
    int PagesForIPCDataStruct = sizeof(InterpreterIPCData) / PageSize + 1;
    InterpreterIPCData *IPCBuffer = (InterpreterIPCData *)RequestPages(PagesForIPCDataStruct);

    int IPC_ID = IPC(IPC_CREATE, IPC_TYPE_MessagePassing, 0, 0, "LOAD", sizeof(InterpreterIPCData));
    while (true)
    {
        IPC(IPC_LISTEN, IPC_TYPE_MessagePassing, IPC_ID, 1, NULL, 0);
        IPC(IPC_WAIT, IPC_TYPE_MessagePassing, IPC_ID, 0, NULL, 0);
        int IPCResult = IPC(IPC_READ, IPC_TYPE_MessagePassing, IPC_ID, 0, IPCBuffer, PageSize);
        if (IPCResult == IPC_E_CODE_Success)
            break;
    }

    struct LibAddressCollection *LibsForLazyResolver = (struct LibAddressCollection *)RequestPages(sizeof(struct LibAddressCollection) / PageSize + 1);
    for (size_t i = 0; i < 64; i++)
    {
        if (IPCBuffer->Libraries[i].Name[0] == '\0')
            break;

        uintptr_t lib_addr = KernelCTL(KCTL_GET_ELF_LIB_FILE, (uint64_t)IPCBuffer->Libraries[i].Name, 0, 0, 0);
        uintptr_t lib_mm_image = KernelCTL(KCTL_GET_ELF_LIB_FILE, (uint64_t)IPCBuffer->Libraries[i].Name, 0, 0, 0);
        if (lib_addr == 0 || lib_mm_image == 0)
        {
            enum SyscallsErrorCodes ret = KernelCTL(KCTL_REGISTER_ELF_LIB, (uint64_t)IPCBuffer->Libraries[i].Name, (uint64_t)IPCBuffer->Libraries[i].Name, 0, 0);
            if (ret != SYSCALL_OK)
            {
                PrintNL("Failed to register ELF lib");
                return -0x11B;
            }
            lib_addr = KernelCTL(KCTL_GET_ELF_LIB_FILE, (uint64_t)IPCBuffer->Libraries[i].Name, 0, 0, 0);
            lib_mm_image = KernelCTL(KCTL_GET_ELF_LIB_FILE, (uint64_t)IPCBuffer->Libraries[i].Name, 0, 0, 0);
        }

        if (LibsForLazyResolver->Next == NULL)
        {
            LibsForLazyResolver->Valid = true;
            LibsForLazyResolver->ElfFile = (uintptr_t)lib_addr;
            LibsForLazyResolver->MemoryImage = (uintptr_t)lib_mm_image;
            LibsForLazyResolver->ParentElfFile = (uintptr_t)IPCBuffer->ElfFile;
            LibsForLazyResolver->ParentMemoryImage = (uintptr_t)IPCBuffer->MemoryImage;
            for (size_t j = 0; j < 32; j++)
                LibsForLazyResolver->Name[j] = IPCBuffer->Libraries[i].Name[j];

            LibsForLazyResolver->Next = (struct LibAddressCollection *)RequestPages(sizeof(struct LibAddressCollection) / PageSize + 1);
            memset(LibsForLazyResolver->Next, 0, sizeof(struct LibAddressCollection));
            continue;
        }
        struct LibAddressCollection *CurrentLibsForLazyResolver = LibsForLazyResolver;

        while (CurrentLibsForLazyResolver->Next != NULL)
            CurrentLibsForLazyResolver = CurrentLibsForLazyResolver->Next;

        CurrentLibsForLazyResolver->Valid = true;
        CurrentLibsForLazyResolver->ElfFile = (uintptr_t)lib_addr;
        CurrentLibsForLazyResolver->MemoryImage = (uintptr_t)lib_mm_image;
        LibsForLazyResolver->ParentElfFile = (uintptr_t)IPCBuffer->ElfFile;
        LibsForLazyResolver->ParentMemoryImage = (uintptr_t)IPCBuffer->MemoryImage;
        for (size_t j = 0; j < 32; j++)
            CurrentLibsForLazyResolver->Name[j] = IPCBuffer->Libraries[i].Name[j];

        CurrentLibsForLazyResolver->Next = (struct LibAddressCollection *)RequestPages(sizeof(struct LibAddressCollection) / PageSize + 1);
        memset(CurrentLibsForLazyResolver->Next, 0, sizeof(struct LibAddressCollection));
    }

    struct LibAddressCollection *CurrentLibsForLazyResolver = LibsForLazyResolver;

    if (!ELFAddLazyResolverToGOT(IPCBuffer->ElfFile, IPCBuffer->MemoryImage, LibsForLazyResolver))
    {
        PrintNL("Failed to add lazy resolver to GOT");
        return -0x607;
    }

    Elf64_Addr Entry = ((Elf64_Ehdr *)IPCBuffer->ElfFile)->e_entry;

    IPC(IPC_DELETE, IPC_TYPE_MessagePassing, IPC_ID, 0, NULL, 0);
    FreePages((uintptr_t)IPCBuffer, PagesForIPCDataStruct);

    return ((int (*)(int, char *[], char *[]))Entry)(argc, argv, envp);
}
