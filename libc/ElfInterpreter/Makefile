# Config file
include ../../../Makefile.conf

NAME=ld

OBJECT_NAME=$(NAME).so
SO_NAME=$(OBJECT_NAME) 

OUTPUT_DIR=../../out/system/
SYSROOT = --sysroot=../../out/system/

CC = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)gcc
AS = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)as
AR = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)ar
OBJDUMP = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)objdump
NASM = /usr/bin/nasm

C_SOURCES = $(shell find ./ -type f -name '*.c')
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp')
S_SOURCES = $(shell find ./ -type f -name '*.S')
ASM_SOURCES = $(shell find ./ -type f -name '*.asm')
OBJ = ${C_SOURCES:.c=.o} ${CPP_SOURCES:.cpp=.o} ${ASM_SOURCES:.asm=.o} ${S_SOURCES:.S=.o}

INCLUDE = ../include

ifeq ($(OSARCH), amd64)
ASM_ARCH := elf64
else ifeq ($(OSARCH), i686)
ASM_ARCH := elf32
endif

CFLAGS := -pie -fPIE -I$(INCLUDE)
LDFLAGS := -nostartfiles -nostdlib -pie -fPIE -Wl,-e_ld_start,-soname,$(SO_NAME) $(SYSROOT)

build: $(OBJECT_NAME)

$(OBJECT_NAME): $(OBJ)
	$(info Linking $@)
	$(CC) $(LDFLAGS) $(OBJ) -o $(OUTPUT_DIR)$@
	$(OBJDUMP) -d $(OUTPUT_DIR)$@ > file_dump.map

%.o: %.c
	$(info Compiling $<)
	$(CC) $(CFLAGS) -std=c17 -c $< -o $@

%.o: %.cpp
	$(info Compiling $<)
	$(CC) $(CFLAGS) -std=c++20 -c $< -o $@

%.o: %.S
	$(info Compiling $<)
	$(AS) -c $< -o $@

%.o: %.asm
	$(info Compiling $<)
	$(NASM) $< -f $(ASM_ARCH) -o $@

clean:
	rm -f file_dump.map $(OBJ)
