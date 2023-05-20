build:
	cp -r include/* ../out/usr/include
	cp ../../Kernel/syscalls.h ../out/usr/include/sys
	make -C runtime build
	make -C src build
	make -C ElfInterpreter build

clean:
	make -C runtime clean
	make -C src clean
	make -C ElfInterpreter clean
