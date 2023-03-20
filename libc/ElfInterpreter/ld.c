#include "ld.h"

#include "../../../Kernel/syscalls.h"
#include "../../../Kernel/ipc.h"
#include "elf.h"

enum KCtl
{
    KCTL_NULL,
    KCTL_GET_PID,
    KCTL_GET_TID,
    KCTL_GET_UID,
    KCTL_GET_GID,
    KCTL_GET_PAGE_SIZE,
    KCTL_IS_CRITICAL,
};

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
    return syscall6(_IPC, Command, Type, ID, Flags, Buffer, Size);
}

uintptr_t KernelCTL(enum KCtl Command, uint64_t Arg1, uint64_t Arg2, uint64_t Arg3, uint64_t Arg4)
{
    return syscall5(_KernelCTL, Command, Arg1, Arg2, Arg3, Arg4);
}

struct LibAddressCollection
{
    char Name[32];
    __UINTPTR_TYPE__ *ElfFile;
    __UINTPTR_TYPE__ *MemoryImage;
    __UINTPTR_TYPE__ *ParentElfFile;
    __UINTPTR_TYPE__ *ParentMemoryImage;
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

                         "mov %r11, %rdi\n"
                         "mov %r10, %rsi\n"

                         "call ELF_LAZY_RESOLVE_MAIN\n"

                         "mov %rax, %r11\n"

                         "pop %r9\n"
                         "pop %r8\n"
                         "pop %rcx\n"
                         "pop %rdx\n"
                         "pop %rsi\n"
                         "pop %rdi\n"

                         "jmp *%r11\n");
}

long abs(long i) { return i < 0 ? -i : i; }

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

    long n = abs(Value);
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

void *memcpy(void *dest, const void *src, __SIZE_TYPE__ n)
{
    __UINT8_TYPE__ *d = dest;
    const __UINT8_TYPE__ *s = src;
    while (n--)
        *d++ = *s++;
    return dest;
}

int strcmp(const char *l, const char *r)
{
    for (; *l == *r && *l; l++, r++)
        ;
    return *(unsigned char *)l - *(unsigned char *)r;
}

Elf64_Sym *ELFGetSymbol(__UINTPTR_TYPE__ *ElfFile, char *SymbolName)
{
    struct Elf64_Dyn *_SymTab = ELFGetDynamicTag(ElfFile, DT_SYMTAB);
    struct Elf64_Dyn *_StrTab = ELFGetDynamicTag(ElfFile, DT_STRTAB);
    Elf64_Sym *DynSym = (Elf64_Sym *)(ElfFile + _SymTab->d_un.d_ptr);
    char *DynStr = (char *)(ElfFile + _StrTab->d_un.d_ptr);

    for (int i = 0; i < _SymTab->d_un.d_val; i++)
    {
        if (strcmp(DynStr + DynSym[i].st_name, SymbolName) == 0)
            return &DynSym[i];
    }
    return (Elf64_Sym *)0;
}

void Print(char *String)
{
    for (short i = 0; String[i] != '\0'; i++)
        PutCharToKernelConsole(String[i]);
}

void PrintNL(char *String)
{
    for (short i = 0; String[i] != '\0'; i++)
        PutCharToKernelConsole(String[i]);

    PutCharToKernelConsole('\n');
}

void (*ELF_LAZY_RESOLVE_MAIN(struct LibAddressCollection *Info, long RelIndex))()
{
    if (Info)
    {
        struct LibAddressCollection *tmp = Info;
        PrintNL("________________");
        // The last entry is the null entry (Valid == false) which determines the end of the list.
        while (tmp->Valid)
        {
            Print("-- ");
            Print(tmp->Name);
            PrintNL(" --");
            __UINTPTR_TYPE__ BaseAddress = __UINTPTR_MAX__;

            Elf64_Phdr ItrProgramHeader;

            for (Elf64_Half i = 0; i < ((Elf64_Ehdr *)tmp->ElfFile)->e_phnum; i++)
            {
                memcpy(&ItrProgramHeader, (__UINT8_TYPE__ *)tmp->ElfFile + ((Elf64_Ehdr *)tmp->ElfFile)->e_phoff + ((Elf64_Ehdr *)tmp->ElfFile)->e_phentsize * i, sizeof(Elf64_Phdr));
                BaseAddress = MIN(BaseAddress, ItrProgramHeader.p_vaddr);
            }

            char LibAddressBuffer[32];
            ltoa(tmp->MemoryImage, LibAddressBuffer, 16);
            Print("MemoryImage: 0x");
            PrintNL(LibAddressBuffer);

            char BaseAddressBuffer[32];
            ltoa(BaseAddress, BaseAddressBuffer, 16);
            Print("BaseAddress: 0x");
            PrintNL(BaseAddressBuffer);

            struct Elf64_Dyn *_JmpRel = ELFGetDynamicTag(tmp->ElfFile, DT_JMPREL);
            struct Elf64_Dyn *_SymTab = ELFGetDynamicTag(tmp->ElfFile, DT_SYMTAB);
            struct Elf64_Dyn *_StrTab = ELFGetDynamicTag(tmp->ElfFile, DT_STRTAB);

            if (!_JmpRel)
            {
                PrintNL("No DT_JMPREL");
                goto RetryNextLib;
            }
            else if (RelIndex >= _JmpRel->d_un.d_val / sizeof(Elf64_Rela))
            {
                PrintNL("RelIndex is greater than the number of relocations");
                goto RetryNextLib;
            }

            if (!_SymTab)
            {
                PrintNL("No DT_SYMTAB");
                goto RetryNextLib;
            }

            if (!_StrTab)
            {
                PrintNL("No DT_STRTAB");
                goto RetryNextLib;
            }

            if (!_JmpRel && !_SymTab && !_StrTab)
                goto RetryNextLib;

            char JmpRel_d_ptr[32];
            ltoa(_JmpRel->d_un.d_ptr, JmpRel_d_ptr, 16);
            Print("JmpRel_d_ptr: 0x");
            PrintNL(JmpRel_d_ptr);

            char SymTab_d_ptr[32];
            ltoa(_SymTab->d_un.d_ptr, SymTab_d_ptr, 16);
            Print("SymTab_d_ptr: 0x");
            PrintNL(SymTab_d_ptr);

            char StrTab_d_ptr[32];
            ltoa(_StrTab->d_un.d_ptr, StrTab_d_ptr, 16);
            Print("StrTab_d_ptr: 0x");
            PrintNL(StrTab_d_ptr);

            Elf64_Rela *JmpRel = tmp->MemoryImage + (_JmpRel->d_un.d_ptr - BaseAddress);
            Elf64_Sym *SymTab = tmp->MemoryImage + (_SymTab->d_un.d_ptr - BaseAddress);
            char *DynStr = tmp->MemoryImage + (_StrTab->d_un.d_ptr - BaseAddress);

            Elf64_Rela *Rel = JmpRel + RelIndex;
            // Elf64_Rela *Rel = &JmpRel[RelIndex];
            Elf64_Addr *GOTEntry = (Elf64_Addr *)(tmp->MemoryImage + Rel->r_offset);

            int RelType = ELF64_R_TYPE(Rel->r_info);

            char RelBuffer[32];
            ltoa(Rel, RelBuffer, 16);
            Print("Rel: 0x");
            PrintNL(RelBuffer);

            char LibRelInfoBuffer[32];
            ltoa(Rel->r_info, LibRelInfoBuffer, 16);
            Print("  Rel->r_info: 0x");
            PrintNL(LibRelInfoBuffer);

            char LibRelOffsetBuffer[32];
            ltoa(Rel->r_offset, LibRelOffsetBuffer, 16);
            Print("  Rel->r_offset: 0x");
            PrintNL(LibRelOffsetBuffer);

            char LibRelAddEntBuffer[32];
            ltoa(Rel->r_addend, LibRelAddEntBuffer, 16);
            Print("  Rel->r_addend: 0x");
            PrintNL(LibRelAddEntBuffer);

            char RelIndexBuffer[32];
            ltoa(RelIndex, RelIndexBuffer, 16);
            Print("RelIndex: 0x");
            PrintNL(RelIndexBuffer);

            char GotAddressBuffer[32];
            ltoa(GOTEntry, GotAddressBuffer, 16);
            Print("GOTEntry: 0x");
            PrintNL(GotAddressBuffer);

            if (GOTEntry && GOTEntry < 0x10000000)
            {
                char GotInsideBuffer[32];
                ltoa(*GOTEntry, GotInsideBuffer, 16);
                Print("*GOTEntry: 0x");
                PrintNL(GotInsideBuffer);
            }

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
                Elf64_Sym *Sym = SymTab + SymIndex;
                // Elf64_Sym *Sym = &SymTab[SymIndex];

                if (Sym->st_name)
                {
                    char *SymName = DynStr + Sym->st_name;
                    PrintNL(SymName);

                    Elf64_Sym *LibSym = ELFGetSymbol(tmp->ElfFile, SymName);
                    if (LibSym)
                    {
                        *GOTEntry = (Elf64_Addr)(tmp->MemoryImage + LibSym->st_value);
                        Lock = 0;
                        return (void (*)()) * GOTEntry;
                    }
                }
                break;
            }
            default:
            {
                char RelTypeBuffer[32];
                ltoa(RelType, RelTypeBuffer, 10);
                Print("RelType not supported ");
                PrintNL(RelTypeBuffer);
                break;
            }
            }

        RetryNextLib:
            tmp = tmp->Next;
        }
    }

    Lock = 0;
    __asm__ __volatile__("mfence\n");

    char SNotFound[32];
    Print("Symbol index ");
    ltoa(RelIndex, SNotFound, 10);
    Print(SNotFound);
    PrintNL(" not found");
    int ExitCode = 0x51801;
    syscall1(_Exit, ExitCode);
    while (1) // Make sure we don't return
        ;
    __builtin_unreachable();
}

bool ELFAddLazyResolverToGOT(void *ElfFile, void *MemoryImage, struct LibAddressCollection *Libs)
{
    struct Elf64_Dyn *Dyn = (struct Elf64_Dyn *)ELFGetDynamicTag(ElfFile, DT_PLTGOT);
    if (!Dyn)
        return false;

    Elf64_Addr *GOT = (Elf64_Addr *)Dyn->d_un.d_ptr;

    // for (size_t i = 0; i < 16; i++)
    // {
    //     char Itr[32];
    //     char LibAddressBuffer[32];
    //     char LibValueBuffer[32];
    //     ltoa(i, Itr, 10);
    //     ltoa(&GOT[i], LibAddressBuffer, 16);
    //     ltoa(GOT[i], LibValueBuffer, 16);

    //     Print("GOT[");
    //     Print(Itr);
    //     Print("]: 0x");
    //     Print(LibAddressBuffer);
    //     Print(" (val: 0x");
    //     Print(LibValueBuffer);
    //     PrintNL(")");
    // }

    GOT[1] = (uintptr_t)Libs;
    GOT[2] = (uintptr_t)ELF_LAZY_RESOLVE_STUB;
    return true;
}

/* This function is a mess and needs to be cleaned up. */
/*
bool ELFDynamicReallocation(void *ElfFile, void *MemoryImage)
{
    debug("ELF dynamic reallocation for image at %#lx.", ElfFile);

    Elf64_Ehdr *ELFHeader = (Elf64_Ehdr *)ElfFile;
    uintptr_t BaseAddress = UINTPTR_MAX;
    size_t ElfAppSize = 0;
    Elf64_Phdr ItrPhdr;

    for (Elf64_Half i = 0; i < ELFHeader->e_phnum; i++)
    {
        memcpy(&ItrPhdr,
               (uint8_t *)ElfFile + ELFHeader->e_phoff + ELFHeader->e_phentsize * i,
               sizeof(Elf64_Phdr));
        BaseAddress = MIN(BaseAddress, ItrPhdr.p_vaddr);
    }

    for (Elf64_Half i = 0; i < ELFHeader->e_phnum; i++)
    {
        memcpy(&ItrPhdr,
               (uint8_t *)ElfFile + ELFHeader->e_phoff + ELFHeader->e_phentsize * i,
               sizeof(Elf64_Phdr));
        uintptr_t SegmentEnd = ItrPhdr.p_vaddr - BaseAddress + ItrPhdr.p_memsz;
        ElfAppSize = MAX(ElfAppSize, SegmentEnd);
    }

    debug("BaseAddress: %#lx Size: %ld", BaseAddress, ElfAppSize);

    Elf64_Dyn *_GOTEntry = (Elf64_Dyn *)ELFGetDynamicTag(ElfFile, DT_PLTGOT);
    Elf64_Dyn *_Rela = ELFGetDynamicTag(ElfFile, DT_RELA);
    Elf64_Dyn *_RelaEnt = ELFGetDynamicTag(ElfFile, DT_RELAENT);
    Elf64_Dyn *_JmpRel = ELFGetDynamicTag(ElfFile, DT_JMPREL);
    Elf64_Dyn *_SymTab = ELFGetDynamicTag(ElfFile, DT_SYMTAB);
    Elf64_Dyn *_StrTab = ELFGetDynamicTag(ElfFile, DT_STRTAB);
    Elf64_Dyn *RelaSize = ELFGetDynamicTag(ElfFile, DT_RELASZ);
    Elf64_Dyn *PltRelSize = ELFGetDynamicTag(ElfFile, DT_PLTRELSZ);

    Elf64_Addr *GOTEntry = (Elf64_Addr *)((uintptr_t)(_GOTEntry->d_un.d_ptr - BaseAddress) + (uintptr_t)MemoryImage);
    Elf64_Dyn *Rela = (Elf64_Dyn *)((uintptr_t)(_Rela->d_un.d_ptr - BaseAddress) + (uintptr_t)MemoryImage);
    Elf64_Dyn *RelaEnt = (Elf64_Dyn *)((uintptr_t)(_RelaEnt->d_un.d_ptr - BaseAddress) + (uintptr_t)MemoryImage);
    Elf64_Dyn *JmpRel = (Elf64_Dyn *)((uintptr_t)(_JmpRel->d_un.d_ptr - BaseAddress) + (uintptr_t)MemoryImage);
    Elf64_Dyn *SymTab = (Elf64_Dyn *)((uintptr_t)(_SymTab->d_un.d_ptr - BaseAddress) + (uintptr_t)MemoryImage);
    Elf64_Dyn *StrTab = (Elf64_Dyn *)((uintptr_t)(_StrTab->d_un.d_ptr - BaseAddress) + (uintptr_t)MemoryImage);

    debug("GOTEntry: %#lx [%#lx]", _GOTEntry, GOTEntry);
    debug("Rela: %#lx [%#lx]", _Rela, Rela);
    debug("RelaEnt: %#lx [%#lx]", _RelaEnt, RelaEnt);
    debug("JmpRel: %#lx [%#lx]", _JmpRel, JmpRel);
    debug("SymTab: %#lx [%#lx]", _SymTab, SymTab);
    debug("StrTab: %#lx [%#lx]", _StrTab, StrTab);
    if (RelaSize)
        debug("RelaSize: %ld", RelaSize->d_un.d_val);
    if (PltRelSize)
        debug("PltRelSize: %ld", PltRelSize->d_un.d_val);

    Elf64_Xword PltRelSizeVal = PltRelSize ? PltRelSize->d_un.d_val : 0;
    Elf64_Xword RelaSizeVal = RelaSize ? RelaSize->d_un.d_val : 0;

    Elf64_Xword PltRelSizeValCount = PltRelSizeVal / sizeof(Elf64_Rela);
    Elf64_Xword RelaSizeValCount = RelaSizeVal / sizeof(Elf64_Rela);

    debug("PltRelSizeVal: %ld", PltRelSizeVal);
    debug("RelaSizeVal: %ld", RelaSizeVal);
    debug("PltRelSizeValCount: %ld", PltRelSizeValCount);
    debug("RelaSizeValCount: %ld", RelaSizeValCount);

    for (Elf64_Xword i = 0; i < PltRelSizeValCount; i++)
    {
        Elf64_Rela *RelaF = (Elf64_Rela *)((uintptr_t)JmpRel + i);
        Elf64_Xword RelaType = ELF64_R_TYPE(RelaF->r_info);
        debug("Itr %ld Type %ld", i, RelaType);

        switch (RelaType)
        {
        case R_X86_64_NONE:
        {
            debug("No relocation needed");
            break;
        }
        case R_X86_64_JUMP_SLOT:
        {
            debug("Relocation for jump slot");
            Elf64_Xword SymIndex = ELF64_R_SYM(RelaF->r_info);
            Elf64_Sym *Sym = (Elf64_Sym *)((uintptr_t)SymTab + SymIndex);
            char *SymName = (char *)((uintptr_t)StrTab + Sym->st_name);
            debug("Symbol %s at %#lx", SymName, Sym->st_value);

            Elf64_Addr *GOTEntry = (Elf64_Addr *)RelaF->r_offset;
            if (Sym->st_value)
            {
                fixme("Not implemented");
                *GOTEntry = (Elf64_Addr)ElfFile + Sym->st_value;
            }
            // else
            // *GOTEntry = (Elf64_Addr)ElfLazyResolver;

            // Elf64_Sym *Sym = (Elf64_Sym *)((uintptr_t)ElfFile + (uintptr_t)SymTab + ELF64_R_SYM(RelaF->r_info) * sizeof(Elf64_Sym));
            // char *SymName = (char *)((uintptr_t)ElfFile + (uintptr_t)StrTab + Sym->st_name);
            // void *SymAddr = (void *)Lib->Address + Sym->st_value;
            // debug("Symbol %s at %#lx", SymName, SymAddr);
            // *(void **)(RelaF->r_offset + (uintptr_t)ElfFile) = SymAddr;
            break;
        }
        case R_X86_64_RELATIVE:
        {
            debug("Relative relocation");
            uintptr_t *Ptr = (uintptr_t *)((uintptr_t)ElfFile + RelaF->r_offset);
            *Ptr = (uintptr_t)MemoryImage + RelaF->r_addend;
            break;
        }
        default:
        {
            fixme("RelaType %d", RelaType);
            break;
        }
        }
    }

    for (Elf64_Xword i = 0; i < RelaSizeValCount; i++)
    {
        Elf64_Rela *RelaF = (Elf64_Rela *)((uintptr_t)ElfFile + (uintptr_t)Rela + i);
        Elf64_Xword RelaType = ELF64_R_TYPE(RelaF->r_info);
        debug("Itr %ld Type %ld", i, RelaType);

        switch (RelaType)
        {
        case R_X86_64_NONE:
        {
            debug("No relocation needed");
            break;
        }
        case R_X86_64_64:
        {
            debug("64-bit relocation");
            Elf64_Xword SymIndex = ELF64_R_SYM(RelaF->r_info);
            Elf64_Sym *Sym = (Elf64_Sym *)((uintptr_t)ElfFile + (uintptr_t)SymTab + SymIndex);
            char *SymName = (char *)((uintptr_t)ElfFile + (uintptr_t)StrTab + Sym->st_name);
            debug("Symbol %s at %#lx", SymName, Sym->st_value);

            uintptr_t *Ptr = (uintptr_t *)((uintptr_t)ElfFile + RelaF->r_offset);
            *Ptr = (uintptr_t)MemoryImage + Sym->st_value + RelaF->r_addend;
            break;
        }
        case R_X86_64_GLOB_DAT:
        {
            debug("Global data relocation");
            Elf64_Xword SymIndex = ELF64_R_SYM(RelaF->r_info);
            Elf64_Sym *Sym = (Elf64_Sym *)((uintptr_t)ElfFile + (uintptr_t)SymTab + SymIndex);
            char *SymName = (char *)((uintptr_t)ElfFile + (uintptr_t)StrTab + Sym->st_name);
            debug("Symbol %s at %#lx", SymName, Sym->st_value);

            uintptr_t *Ptr = (uintptr_t *)((uintptr_t)ElfFile + RelaF->r_offset);
            *Ptr = (uintptr_t)MemoryImage + Sym->st_value;
            break;
        }
        case R_X86_64_RELATIVE:
        {
            debug("Relative relocation");
            Elf64_Xword SymIndex = ELF64_R_SYM(RelaF->r_info);
            Elf64_Sym *Sym = (Elf64_Sym *)((uintptr_t)ElfFile + (uintptr_t)SymTab + SymIndex);
            char *SymName = (char *)((uintptr_t)ElfFile + (uintptr_t)StrTab + Sym->st_name);
            debug("Symbol %s at %#lx", SymName, Sym->st_value);

            uintptr_t *Ptr = (uintptr_t *)((uintptr_t)ElfFile + RelaF->r_offset);
            *Ptr = (uintptr_t)MemoryImage + RelaF->r_addend;
            break;
        }
        default:
        {
            fixme("RelaType %d", RelaType);
            break;
        }
        }
    }
    return true;
}
*/

/*

        ELFDynamicReallocation(ElfFile, MemoryImage);

        LibAddressCollection *LibsForLazyResolver = (LibAddressCollection *)ELFBase.TmpMem->RequestPages(TO_PAGES(sizeof(LibAddressCollection)), true);
        memset(LibsForLazyResolver, 0, sizeof(LibAddressCollection));
        LibAddressCollection *LFLRTmp = LibsForLazyResolver;
        debug("LibsForLazyResolver: %#lx", LibsForLazyResolver);

        if (NeededLibraries.size() > 0)
        {
            VirtualFileSystem::Node *ParentNode = ExFile->Node->Parent; // Working Directory
            if (ParentNode)
            {
                char *WorkingDirAbsolutePath = vfs->GetPathFromNode(ParentNode);
                debug("Working directory: \"%s\"", WorkingDirAbsolutePath);

                int LibCount = 0;
                foreach (auto Library in NeededLibraries)
                {
                    char LibPath[256];
                    strcpy(LibPath, WorkingDirAbsolutePath);
                    strcat(LibPath, "/");
                    strcat(LibPath, Library);
                    debug("Searching for \"%s\"...", LibPath);

                    bool AlreadyTried = false;

                LibPathRetry:
                    VirtualFileSystem::FILE *LibNode = vfs->Open(LibPath);

                    if (LibNode->Status != VirtualFileSystem::FileStatus::OK)
                    {
                        vfs->Close(LibNode);
                        if (!AlreadyTried)
                        {
                            debug("Library \"%s\" not found, retrying... (%#x)", LibPath, LibNode->Status);
                            memset(LibPath, 0, 256);
                            strcpy(LibPath, "/system/lib/");
                            strcat(LibPath, Library);
                            AlreadyTried = true;
                            goto LibPathRetry;
                        }
                        else
                            warn("Failed to load library \"%s\" (%#x)", Library, LibNode->Status);
                    }
                    else
                    {
                        debug("Library found \"%s\" (%#x)", LibPath, LibNode->Status);
                        SharedLibraries *sl = AddLibrary(Library, (void *)LibNode->Node->Address, LibNode->Node->Length);
                        strcpy(LFLRTmp->Name, Library);
                        LFLRTmp->ElfFile = (uintptr_t *)sl->Address;
                        LFLRTmp->MemoryImage = (uintptr_t *)sl->MemoryImage;
                        LFLRTmp->ParentElfFile = (uintptr_t *)ElfFile;
                        LFLRTmp->ParentMemoryImage = (uintptr_t *)MemoryImage;
                        LFLRTmp->Valid = true;
                        debug("LIBRARY: %s, %#lx, %#lx", Library, LFLRTmp->ElfFile, LFLRTmp->MemoryImage);

                        LFLRTmp->Next = (LibAddressCollection *)ELFBase.TmpMem->RequestPages(TO_PAGES(sizeof(LibAddressCollection)), true);
                        LFLRTmp = LFLRTmp->Next;
                        memset(LFLRTmp, 0, sizeof(LibAddressCollection));
                    }
                }
            }
            else
            {
                error("Couldn't get the parent node from path %s", vfs->GetPathFromNode(ExFile->Node));
            }
        }

        ELFAddLazyResolverToGOT(ElfFile, MemoryImage, LibsForLazyResolver);
*/

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

    syscall2(_Print, 'H', 0);

    /* Everything is ok, continue. */
    return 0;
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

/* Actual load */
int ld_load(int argc, char *argv[], char *envp[])
{
    syscall2(_Print, 'L', 0);

    uintptr_t PageSize = KernelCTL(KCTL_GET_PAGE_SIZE, 0, 0, 0, 0);
    int PagesForStruct = sizeof(InterpreterIPCData) / PageSize + 1;
    InterpreterIPCData *IPCBuffer = (InterpreterIPCData *)RequestPages(PagesForStruct);

    int IPC_ID = IPC(IPC_CREATE, IPC_TYPE_MessagePassing, 0, 0, "LOAD", sizeof(InterpreterIPCData));
    while (1)
    {
        IPC(IPC_LISTEN, IPC_TYPE_MessagePassing, IPC_ID, 1, NULL, 0);
        IPC(IPC_WAIT, IPC_TYPE_MessagePassing, IPC_ID, 0, NULL, 0);
        int IPCResult = IPC(IPC_READ, IPC_TYPE_MessagePassing, IPC_ID, 0, IPCBuffer, PageSize);
        if (IPCResult == IPC_E_CODE_Success)
            break;
    }

    for (size_t i = 0; i < 256; i++)
    {
        if (IPCBuffer->Path[i] == '\0')
            break;
        syscall2(_Print, IPCBuffer->Path[i], 0);
    }
    syscall2(_Print, '\n', 0);

    IPC(IPC_DELETE, IPC_TYPE_MessagePassing, IPC_ID, 0, NULL, 0);
    FreePages((uintptr_t)IPCBuffer, PagesForStruct);

    Elf64_Ehdr *ELFHeader = (Elf64_Ehdr *)IPCBuffer->MemoryImage;

    /* ... */

    struct LibAddressCollection *LibsForLazyResolver = (struct LibAddressCollection *)RequestPages(sizeof(struct LibAddressCollection) / PageSize + 1);
    /*
    TODO: Add libraries to LibsForLazyResolver
    This can be done by calling the kernel to load the libraries, and then adding them to the LibsForLazyResolver struct.
    Kernel has a thread for loading libraries. If the lib is already loaded, it will return the address of the loaded lib.
    */

    if (!ELFAddLazyResolverToGOT(IPCBuffer->ElfFile, IPCBuffer->MemoryImage, LibsForLazyResolver))
    {
        for (size_t i = 0; i < 35; i++)
            syscall2(_Print, "Failed to add lazy resolver to GOT\n"[i], 0);
        return -0xE1F;
    }

    return ((int (*)(int, char *[], char *[]))ELFHeader->e_entry)(argc, argv, envp);
}
