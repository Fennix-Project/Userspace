build:
	cp include/* ../out/system/include
	make --quiet -C runtime build
	make --quiet -C libgcc build
	make --quiet -C src build

clean:
	make --quiet -C runtime clean
	make --quiet -C libgcc clean
	make --quiet -C src clean
