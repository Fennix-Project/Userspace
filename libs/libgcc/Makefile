# Config file
include ../../../Makefile.conf

NAME=gcc

OBJECT_NAME=lib$(NAME).a

OUTPUT_DIR=../../out/system/lib/

CC = ../../../$(TC_COMPILER_PATH)/$(TC_COMPILER_ARCH)gcc

CRTBEGIN_PATH = $(shell $(CC) -print-file-name=libgcc.a)

build: $(OBJECT_NAME)

$(OBJECT_NAME):
	cp $(CRTBEGIN_PATH) $(OUTPUT_DIR)

clean:

