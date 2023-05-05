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
INCLUDE2 = ../../out/system/include

ifeq ($(OSARCH), amd64)
ASM_ARCH := elf64
else ifeq ($(OSARCH), i386)
ASM_ARCH := elf32
endif

CFLAGS := -fPIC -I$(INCLUDE) -I$(INCLUDE2)

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -ggdb3 -O0 -fdiagnostics-color=always -fverbose-asm
#	LDFLAGS += -ggdb3 -O0
endif

build: $(OBJECT_NAME)

$(OBJECT_NAME): $(OBJ)
	$(info Linking $@)
#	$(CC) -nostdlib -shared -fPIC -fPIE -Wl,-soname,libc.so $(OBJ) -o $(OUTPUT_DIR)$@
	$(AR) rcs $(OUTPUT_DIR)$@ $(OBJ)

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
	rm -f $(OBJ) file_dump.map
