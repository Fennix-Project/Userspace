# Config file
include ../Makefile.conf

cwd := $(CURDIR)
PREFIX := $(cwd)/out/
TARGET := x86_64-fennix

export CROSS_COMPILE := $(cwd)/../tools/cross/bin/$(TARGET)-
export CC := $(cwd)/../tools/cross/bin/$(TARGET)-gcc
export LD := $(cwd)/../tools/cross/bin/$(TARGET)-ld
export AR := $(cwd)/../tools/cross/bin/$(TARGET)-ar
export STRIP := $(cwd)/../tools/cross/bin/$(TARGET)-strip
export RANLIB := $(cwd)/../tools/cross/bin/$(TARGET)-ranlib
export LD_LIBRARY_PATH := $(cwd)/out/lib/

ifeq ($(DEBUG), 1)
export CFLAGS := --sysroot=$(cwd)/out/ -DDEBUG -ggdb3 -O0 -fdiagnostics-color=always -fverbose-asm
export LDFLAGS := -ggdb3 -O0
else
export CFLAGS := --sysroot=$(cwd)/out/
endif

ifeq ($(USERSPACE_STATIC_LIBS), 1)
MUSL_CONFIGURE_FLAGS := --enable-static --disable-shared
MLIBC_CONFIGURE_FLAGS := -Ddefault_library=static
else
MUSL_CONFIGURE_FLAGS := --enable-shared --enable-static
MLIBC_CONFIGURE_FLAGS := -Ddefault_library=both
endif

build_musl:
	mkdir -p cache/musl
	cd cache/musl && \
	../../musl/configure --prefix=$(PREFIX) \
	--target=$(TARGET) --includedir=$(PREFIX)/include \
	$(MUSL_CONFIGURE_FLAGS)
	make -C cache/musl -j$(shell nproc)
	cd cache/musl && make TARGET=$(TARGET) install
	cp out/lib/crt1.o out/lib/crt0.o
	cd out/lib && ln -s /lib/libc.so ./ld-musl-x86_64.so.1 && \
	ln -s /lib/libc.so ./ld.so
	mkdir -p out/include/fennix
	cp ../Kernel/syscalls.h out/include/fennix/syscall.h

build_mlibc:
	cp ../Kernel/syscalls.h ./mlibc/sysdeps/fennix/include/fennix/syscall.h
ifeq ($(wildcard cache/mlibc),)
	cd mlibc && meson $(MLIBC_CONFIGURE_FLAGS) --cross-file ci/fennix.cross-file ../cache/mlibc
endif
	cd cache/mlibc && DESTDIR=../../out ninja install
	mv out/usr/local/include/* out/include/
	mv out/usr/local/lib/* out/lib/

create_out:
	rm -rf out
	mkdir -p out
	mkdir -p out/bin
	mkdir -p out/lib
	mkdir -p out/include
	mkdir -p out/usr/bin
	mkdir -p out/usr/share
	mkdir -p out/usr/share/doc
	mkdir -p out/usr/share/info
	mkdir -p out/usr/include

build: create_out
ifeq ($(USE_LIBC), internal)
	make -C libc build
else ifeq ($(USE_LIBC), musl)
	$(MAKE) build_musl
else ifeq ($(USE_LIBC), mlibc)
	$(MAKE) build_mlibc
endif
	make -C libs build
	make -C apps build

prepare:
	$(info Nothing to prepare)

clean:
	rm -rf out cache
	mkdir -p cache
	touch cache/.gitkeep
	make -C libc clean
	make -C libs clean
	make -C apps clean
