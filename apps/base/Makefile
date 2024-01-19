# Config file
include ../../../Makefile.conf

cwd := $(CURDIR)
CACHE_DIR := $(cwd)/../../cache
PREFIX := $(cwd)/../../out/
TARGET := x86_64-fennix

export CC := $(cwd)/../../../tools/cross/bin/$(TARGET)-gcc
export LD := $(cwd)/../../../tools/cross/bin/$(TARGET)-ld
export AR := $(cwd)/../../../tools/cross/bin/$(TARGET)-ar
export STRIP := $(cwd)/../../../tools/cross/bin/$(TARGET)-strip
export RANLIB := $(cwd)/../../../tools/cross/bin/$(TARGET)-ranlib
export LD_LIBRARY_PATH := $(cwd)/../../out/lib/

ifeq ($(DEBUG), 1)
export CFLAGS := --sysroot=$(cwd)/../../out/ -I$(cwd)/../../out/include/ -DDEBUG -ggdb3 -O0 -fdiagnostics-color=always -fverbose-asm
export LDFLAGS := -ggdb3 -O0
else
export CFLAGS := --sysroot=$(cwd)/../../out/ -I$(cwd)/../../out/include/
endif

build_bash:
ifeq ($(wildcard $(CACHE_DIR)/bash),)
	mkdir -p $(CACHE_DIR)/bash
	cd $(CACHE_DIR)/bash && \
	../../apps/base/bash/configure --prefix=$(PREFIX) \
	--host=$(TARGET) \
	--enable-minimal-config
endif
	LDFLAGS="--static" make -C $(CACHE_DIR)/bash -j$(shell nproc)
	make -C $(CACHE_DIR)/bash install

BUSYBOX_CROSS_PATH := $(cwd)/../../../tools/cross/bin/$(TARGET)-

build_busybox:
ifeq ($(wildcard $(CACHE_DIR)/busybox),)
	mkdir -p $(CACHE_DIR)/busybox
	cd $(CACHE_DIR)/busybox && \
	make KBUILD_SRC=../../apps/base/busybox \
	CROSS_COMPILE=$(BUSYBOX_CROSS_PATH) \
	-f ../../apps/base/busybox/Makefile \
	allnoconfig
endif
	cd $(CACHE_DIR)/busybox && \
	make -C $(CACHE_DIR)/busybox \
	CROSS_COMPILE=$(BUSYBOX_CROSS_PATH) -j$(shell nproc)
	cd $(CACHE_DIR)/busybox && \
	make -C $(CACHE_DIR)/busybox \
	CONFIG_PREFIX=$(PREFIX) \
	CROSS_COMPILE=$(BUSYBOX_CROSS_PATH) install

build:
	make -C echo build
	make -C fsh build
	make -C utest build
	make -C cross_test build
#	$(MAKE) build_busybox
#	$(MAKE) build_bash

clean:
	make -C echo clean
	make -C fsh clean
	make -C utest clean
	make -C cross_test clean
