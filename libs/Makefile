build:
	cp -r include/* ../out/include
	make -C libgcc build
	make -C libinit build
	make -C libssp build

clean:
	make -C libgcc clean
	make -C libinit clean
	make -C libssp clean
