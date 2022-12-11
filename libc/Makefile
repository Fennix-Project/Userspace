build:
	cp -r include/* ../out/system/include
	cp ../../Kernel/syscalls.h ../out/system/include/sys
	make --quiet -C runtime build
	make --quiet -C src build

clean:
	make -C runtime clean
	make -C src clean
