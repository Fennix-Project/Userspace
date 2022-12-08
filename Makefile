# Config file
include ../Makefile.conf

build:
ifeq ($(NEWLIB), 1)

ifeq (,$(wildcard ./newlib-cygwin))
	git clone https://github.com/Fennix-Project/newlib-cygwin.git newlib-cygwin
endif

else ifeq ($(MLIBC), 1)

ifeq (,$(wildcard ./newlib-cygwin))
	git clone https://github.com/Fennix-Project/mlibc.git mlibc
endif

else
	mkdir -p out
	mkdir -p out/system
	mkdir -p out/system/lib
	mkdir -p out/system/include
	make --quiet -C libc build
	make --quiet -C libs build
	make --quiet -C apps build
endif

prepare:
	$(info Nothing to prepare)

clean:
	rm -rf out
	make -C libc clean
	make -C libs clean
	make -C apps clean
