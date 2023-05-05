# Config file
include ../../../../Makefile.conf

FILENAME = init.elf

SYSROOT = --sysroot=../../../out/system/

CC = ../../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)gcc
CPP = ../../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)g++
LD = ../../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)ld
AS = ../../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)as
OBJDUMP = ../../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)objdump

GIT_COMMIT = $(shell git rev-parse HEAD)
GIT_COMMIT_SHORT = $(shell git rev-parse --short HEAD)

ifeq ($(OSARCH), amd64)
S_SOURCES = $(shell find ./ -type f -name '*.S' -not -path "./arch/i386/*" -not -path "./arch/aarch64/*")
C_SOURCES = $(shell find ./ -type f -name '*.c' -not -path "./arch/i386/*" -not -path "./arch/aarch64/*")
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp' -not -path "./arch/i386/*" -not -path "./arch/aarch64/*")
else ifeq ($(OSARCH), i386)
S_SOURCES = $(shell find ./ -type f -name '*.S' -not -path "./arch/amd64/*" -not -path "./arch/aarch64/*")
C_SOURCES = $(shell find ./ -type f -name '*.c' -not -path "./arch/amd64/*" -not -path "./arch/aarch64/*")
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp' -not -path "./arch/amd64/*" -not -path "./arch/aarch64/*")
else ifeq ($(OSARCH), aarch64)
S_SOURCES = $(shell find ./ -type f -name '*.S' -not -path "./arch/amd64/*" -not -path "./arch/i386/*")
C_SOURCES = $(shell find ./ -type f -name '*.c' -not -path "./arch/amd64/*" -not -path "./arch/i386/*")
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp' -not -path "./arch/amd64/*" -not -path "./arch/i386/*")
endif
HEADERS = $(sort $(dir $(wildcard ../../../out/system/include/*)))
OBJ = $(C_SOURCES:.c=.o) $(CPP_SOURCES:.cpp=.o) $(ASM_SOURCES:.asm=.o) $(S_SOURCES:.S=.o) $(PSF_SOURCES:.psf=.o) $(BMP_SOURCES:.bmp=.o)
INCLUDE_DIR = ../../../out/system/include

LDFLAGS := -Wl,-Map file.map

WARNCFLAG = -Wall -Wextra -Wno-main

CFLAGS :=										\
	-I$(INCLUDE_DIR)							\
	-DGIT_COMMIT='"$(GIT_COMMIT)"'				\
	-DGIT_COMMIT_SHORT='"$(GIT_COMMIT_SHORT)"'

ifeq ($(OSARCH), amd64)
CFLAGS += -march=x86-64 -fstack-protector-all -fstack-clash-protection
else ifeq ($(OSARCH), i386)
CFLAGS += -march=i386
else ifeq ($(OSARCH), aarch64)
CFLAGS += -pipe
endif

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -ggdb3 -O0 -fdiagnostics-color=always -fverbose-asm
	LDFLAGS += -ggdb3 -O0
endif

build: $(FILENAME)
	$(OBJDUMP) -d $(FILENAME) > file_dump.map
	mv $(FILENAME) ../../../out/system/$(FILENAME)

$(FILENAME): $(OBJ)
	$(info Linking $@)
	$(CC) $(LDFLAGS) $(SYSROOT) $(OBJ) -lssp -linit -o $@

%.o: %.c $(HEADERS)
	$(info Compiling $<)
	$(CC) $(CFLAGS) $(WARNCFLAG) -std=c17 -c $< -o $@

%.o: %.cpp $(HEADERS)
	$(info Compiling $<)
	$(CPP) $(CFLAGS) $(WARNCFLAG) -std=c++20 -c $< -o $@

%.o: %.S
	$(info Compiling $<)
ifeq ($(OSARCH), amd64)
	$(AS) -o $@ $<
else ifeq ($(OSARCH), i386)
	$(AS) -o $@ $<
else ifeq ($(OSARCH), aarch64)
	$(AS) -o $@ $<
endif

clean:
	rm -f *.o file.map file_dump.map $(OBJ)
