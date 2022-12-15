# Config file
include ../../../Makefile.conf

NAME=c

OBJECT_NAME=lib$(NAME).a

OUTPUT_DIR=../../out/system/lib/

CC = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)gcc
AS = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)as
AR = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)ar
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

CFLAGS := -fPIC -I$(INCLUDE)

build: $(OBJECT_NAME)

$(OBJECT_NAME): $(OBJ)
	$(AR) rcs $(OUTPUT_DIR)$@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -std=c17 -c $< -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -std=c++20 -c $< -o $@

%.o: %.S
	$(AS) -c $< -o $@

%.o: %.asm
	$(NASM) $< -f $(ASM_ARCH) -o $@

clean:
	rm -f $(OBJ)
