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
};

// bool ELFAddLazyResolverToGOT(void *ElfFile, void *MemoryImage, LibAddressCollection *Libs)
// {
//     struct Elf64_Dyn *Dyn = (struct Elf64_Dyn *)ELFGetDynamicTag(ElfFile, DT_PLTGOT);
//     if (!Dyn)
//         return false;

//     Elf64_Addr *GOT = (Elf64_Addr *)Dyn->d_un.d_ptr;

//     for (size_t i = 0; i < 16; i++)
//         debug("GOT[%d]: %#lx (val: %#lx)", i, &GOT[i], GOT[i]);

//     GOT[1] = (uintptr_t)Libs;
//     GOT[2] = (uintptr_t)ElfLazyResolver;
//     return true;
// }

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
void ld_main()
{
    __asm__ __volatile__("syscall"
                         :
                         : "a"(1), "D"('H'), "S"(0)
                         : "rcx", "r11", "memory");

    return;
}

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

/* Actual load */
int ld_load(int argc, char *argv[], char *envp[])
{
    __asm__ __volatile__("syscall"
                         :
                         : "a"(1), "D"('L'), "S"(0)
                         : "rcx", "r11", "memory");

    void *IPCBuffer = (void *)RequestPages(1);

    uintptr_t PageSize = KernelCTL(KCTL_GET_PAGE_SIZE, 0, 0, 0, 0);

    int IPC_ID = IPC(IPC_CREATE, IPC_TYPE_MessagePassing, 0, 0, "LOAD", PageSize);

    while (1)
    {
        IPC(IPC_LISTEN, IPC_TYPE_MessagePassing, IPC_ID, 1, NULL, 0);
        IPC(IPC_WAIT, IPC_TYPE_MessagePassing, IPC_ID, 0, NULL, 0);
        int IPCResult = IPC(IPC_READ, IPC_TYPE_MessagePassing, IPC_ID, 0, IPCBuffer, PageSize);
        if (IPCResult == IPC_E_CODE_Success)
            break;
    }

    IPC(IPC_DELETE, IPC_TYPE_MessagePassing, IPC_ID, 0, NULL, 0);
    // FreePages((uintptr_t)IPCBuffer, 1); <- The kernel will free the buffer for us
    return *(uintptr_t *)IPCBuffer;
}
