# Config file
include ../../../Makefile.conf

CC = ../../../$(COMPILER_PATH)/$(COMPILER_ARCH)gcc
NASM = /usr/bin/nasm

C_SOURCES = $(shell find ./ -type f -name '*.c')
S_SOURCES = $(shell find ./ -type f -name '*.s')
ASM_SOURCES = $(shell find ./ -type f -name '*.asm')
OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.o} ${S_SOURCES:.s=.o}

ifeq ($(OSARCH), amd64)
ASM_ARCH := elf64
else ifeq ($(OSARCH), i686)
ASM_ARCH := elf32
endif

build: $(OBJ)
	mv $^ ../../out/system/lib/

%.o: %.c
	$(CC) -nostdlib -std=c17 -c $< -o $@

%.o: %.asm
	$(NASM) $< -f $(ASM_ARCH) -o $@

%.bin: %.s
	$(NASM) $< -f $(ASM_ARCH) -o $@

clean:
