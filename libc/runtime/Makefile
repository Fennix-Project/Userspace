# Config file
include ../../../Makefile.conf

CC = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)gcc
AS = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)as
NASM = /usr/bin/nasm

C_SOURCES = $(shell find ./ -type f -name '*.c')
S_SOURCES = $(shell find ./ -type f -name '*.S')
ASM_SOURCES = $(shell find ./ -type f -name '*.asm')
OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.o} ${S_SOURCES:.S=.o}

ifeq ($(OSARCH), amd64)
ASM_ARCH := elf64
else ifeq ($(OSARCH), i686)
ASM_ARCH := elf32
endif

CRTBEGIN_PATH = $(shell $(CC) -print-file-name=crtbegin.o)
CRTEND_PATH = $(shell $(CC) -print-file-name=crtend.o)
CRTI_PATH = $(shell $(CC) -print-file-name=crti.o)
CRTN_PATH = $(shell $(CC) -print-file-name=crtn.o)

build: $(OBJ)
	mv $^ ../../out/system/lib/
	cp $(CRTBEGIN_PATH) $(CRTEND_PATH) $(CRTI_PATH) $(CRTN_PATH) ../../out/system/lib/

%.o: %.c
	$(info Compiling $<)
	$(CC) -nostdlib -mno-red-zone -std=c17 -c $< -o $@

%.o: %.asm
	$(info Compiling $<)
	$(NASM) $< -f $(ASM_ARCH) -o $@

%.o: %.S
	$(info Compiling $<)
	$(AS) -c $< -o $@

clean:

