# Config file
include ../../../Makefile.conf

NAME=ssp

OBJECT_NAME=lib$(NAME).a

OUTPUT_DIR=../../out/system/lib/

CC = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)gcc
AR = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)ar
NASM = /usr/bin/nasm

C_SOURCES = $(shell find ./ -type f -name '*.c')
ASM_SOURCES = $(shell find ./ -type f -name '*.asm')
OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.o}

ifeq ($(OSARCH), amd64)
ASM_ARCH := elf64
else ifeq ($(OSARCH), i686)
ASM_ARCH := elf32
endif

CFLAGS := -fPIC -I../include -I../../libc/include

build: $(OBJECT_NAME)

$(OBJECT_NAME): $(OBJ)
	$(AR) rcs $(OUTPUT_DIR)$@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -std=c17 -c $< -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -std=c++20 -c $< -o $@

%.o: %.asm
	$(NASM) $< -f $(ASM_ARCH) -o $@

clean:
	rm -f $(OBJ)