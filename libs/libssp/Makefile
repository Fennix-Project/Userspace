# Config file
include ../../../Makefile.conf

NAME=ssp

OBJECT_NAME=lib$(NAME).a

OUTPUT_DIR=../../out/lib/

CC = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)gcc
AR = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)ar
OBJDUMP = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)objdump
NASM = /usr/bin/nasm

C_SOURCES = $(shell find ./ -type f -name '*.c')
ASM_SOURCES = $(shell find ./ -type f -name '*.asm')
OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.o}

ifeq ($(OSARCH), amd64)
ASM_ARCH := elf64
else ifeq ($(OSARCH), i386)
ASM_ARCH := elf32
endif

CFLAGS := -fPIC -I../include -I../../libc/include

build: $(OBJECT_NAME)

$(OBJECT_NAME): $(OBJ)
	$(info Linking $@)
	$(AR) rcs $(OUTPUT_DIR)$@ $(OBJ)
	$(OBJDUMP) -d $(OUTPUT_DIR)$@ > file_dump.map

%.o: %.c
	$(info Compiling $<)
	$(CC) $(CFLAGS) -std=c17 -c $< -o $@

%.o: %.cpp
	$(info Compiling $<)
	$(CC) $(CFLAGS) -std=c++20 -c $< -o $@

%.o: %.asm
	$(info Compiling $<)
	$(NASM) $< -f $(ASM_ARCH) -o $@

clean:
	rm -f file_dump.map $(OBJ)
