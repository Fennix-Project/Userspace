# Config file
include ../Makefile.conf

build:
ifeq ($(NEWLIB),1)
	make -C mlibc build
else
	mkdir -p out
	mkdir -p out/lib
	mkdir -p out/bin
	mkdir -p out/usr/include
	make -C libc build
	make -C libs build
	make -C apps build
endif

prepare:
	$(info Nothing to prepare)

clean:
	rm -rf out
	make -C libc clean
	make -C libs clean
	make -C apps clean
