# Config file
include ../../../../Makefile.conf

FILENAME = echo.elf

SYSROOT = --sysroot=../../../out/system/

CC = ../../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)gcc
CPP = ../../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)g++
LD = ../../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)ld
AS = ../../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)as
OBJDUMP = ../../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)objdump

GIT_COMMIT = $(shell git rev-parse HEAD)
GIT_COMMIT_SHORT = $(shell git rev-parse --short HEAD)

ifeq ($(OSARCH), amd64)
S_SOURCES = $(shell find ./ -type f -name '*.S' -not -path "./arch/i686/*" -not -path "./arch/aarch64/*")
C_SOURCES = $(shell find ./ -type f -name '*.c' -not -path "./arch/i686/*" -not -path "./arch/aarch64/*")
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp' -not -path "./arch/i686/*" -not -path "./arch/aarch64/*")
else ifeq ($(OSARCH), i686)
S_SOURCES = $(shell find ./ -type f -name '*.S' -not -path "./arch/amd64/*" -not -path "./arch/aarch64/*")
C_SOURCES = $(shell find ./ -type f -name '*.c' -not -path "./arch/amd64/*" -not -path "./arch/aarch64/*")
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp' -not -path "./arch/amd64/*" -not -path "./arch/aarch64/*")
else ifeq ($(OSARCH), aarch64)
S_SOURCES = $(shell find ./ -type f -name '*.S' -not -path "./arch/amd64/*" -not -path "./arch/i686/*")
C_SOURCES = $(shell find ./ -type f -name '*.c' -not -path "./arch/amd64/*" -not -path "./arch/i686/*")
CPP_SOURCES = $(shell find ./ -type f -name '*.cpp' -not -path "./arch/amd64/*" -not -path "./arch/i686/*")
endif
HEADERS = $(sort $(dir $(wildcard ../../../out/system/include/*)))
OBJ = $(C_SOURCES:.c=.o) $(CPP_SOURCES:.cpp=.o) $(ASM_SOURCES:.asm=.o) $(S_SOURCES:.S=.o) $(PSF_SOURCES:.psf=.o) $(BMP_SOURCES:.bmp=.o)
INCLUDE_DIR = ../../../out/system/include

LDFLAGS := -Wl,-Map file.map

WARNCFLAG = -Wall -Wextra -Wno-builtin-declaration-mismatch

CFLAGS :=										\
	-I$(INCLUDE_DIR)							\
	-DGIT_COMMIT='"$(GIT_COMMIT)"'				\
	-DGIT_COMMIT_SHORT='"$(GIT_COMMIT_SHORT)"'

ifeq ($(OSARCH), amd64)

CFLAGS += -march=x86-64

else ifeq ($(OSARCH), i686)

CFLAGS += -march=i686

else ifeq ($(OSARCH), aarch64)

CFLAGS += -pipe

endif

build: $(FILENAME)
	$(OBJDUMP) -d $(FILENAME) > file_dump.map
	mv $(FILENAME) ../../../out/system/$(FILENAME)

$(FILENAME): $(OBJ)
	$(CC) $(LDFLAGS) $(SYSROOT) $(OBJ) -o $@

%.o: %.c $(HEADERS)
	$(info Compiling $<)
	$(CC) $(CFLAGS) $(WARNCFLAG) -std=c17 -c $< -o $@

%.o: %.cpp $(HEADERS)
	$(info Compiling $<)
	$(CPP) $(CFLAGS) $(WARNCFLAG) -std=c++20 -fexceptions -c $< -o $@ -fno-rtti

%.o: %.S
	$(info Compiling $<)
ifeq ($(OSARCH), amd64)
	$(AS) -o $@ $<
else ifeq ($(OSARCH), i686)
	$(AS) -o $@ $<
else ifeq ($(OSARCH), aarch64)
	$(AS) -o $@ $<
endif

clean:
	rm -f *.o file.map file_dump.map $(OBJ)
