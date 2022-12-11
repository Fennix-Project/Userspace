build:
	cp include/* ../out/system/include
	make --quiet -C libgcc build
	make --quiet -C libinit build
	make --quiet -C libssp build

clean:
	make -C libgcc clean
	make -C libinit clean
	make -C libssp clean
