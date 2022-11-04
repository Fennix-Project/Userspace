OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386:x86-64)

ENTRY(_start)

SECTIONS
{
    .header :
    {
        *(.header .header.*)
    }

    .text :
    {
        *(.text .text.*)
    }

    .data :
    {
        *(.data .data.*)
    }

    .rodata :
    {
        *(.rodata .rodata.*)
    }

    .bss :
    {
        *(COMMON)
        *(.bss .bss.*)
    }

    /DISCARD/ :
    {
        *(.eh_frame)
        *(.note .note.*)
    }
}
