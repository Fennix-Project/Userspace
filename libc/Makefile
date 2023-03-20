build:
	cp -r include/* ../out/system/include
	cp ../../Kernel/syscalls.h ../out/system/include/sys
	make -C runtime build
	make -C src build
	make -C ElfInterpreter build

clean:
	make -C runtime clean
	make -C src clean
	make -C ElfInterpreter clean
