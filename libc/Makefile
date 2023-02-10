build:
	cp -r include/* ../out/system/include
	cp ../../Kernel/syscalls.h ../out/system/include/sys
	make --quiet -C runtime build
	make --quiet -C src build
	make --quiet -C ElfInterpreter build

clean:
	make -C runtime clean
	make -C src clean
	make -C ElfInterpreter clean
