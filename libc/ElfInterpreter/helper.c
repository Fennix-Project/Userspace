#include "elf.h"

struct Elf64_Dyn *ELFGetDynamicTag(void *ElfFile, enum DynamicArrayTags Tag)
{
    Elf64_Ehdr *ELFHeader = (Elf64_Ehdr *)ElfFile;

    Elf64_Phdr ItrProgramHeader;
    for (Elf64_Half i = 0; i < ELFHeader->e_phnum; i++)
    {
        memcpy(&ItrProgramHeader, (__UINT8_TYPE__ *)ElfFile + ELFHeader->e_phoff + ELFHeader->e_phentsize * i, sizeof(Elf64_Phdr));
        if (ItrProgramHeader.p_type == PT_DYNAMIC)
        {
            struct Elf64_Dyn *Dynamic = (struct Elf64_Dyn *)((__UINT8_TYPE__ *)ElfFile + ItrProgramHeader.p_offset);
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
