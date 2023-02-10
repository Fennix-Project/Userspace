#include "elf.h"

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

long RelTmpIndex = 0xdead;

void *SymbolNotFound()
{
    // TODO: Print the symbol name and the library name.
    // TODO: This should go to the stderr.
    char Buffer[32];

    Print("Symbol index ");
    ltoa(RelTmpIndex, Buffer, 10);
    Print(Buffer);
    PrintNL(" not found");
    return (void *)0xdeadbeef;
}

void (*ELF_LAZY_RESOLVE_MAIN(struct LibAddressCollection *Info, long RelIndex))()
{
    RelTmpIndex = RelIndex;
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
    return SymbolNotFound;
}
