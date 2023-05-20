build:
	cp -r include/* ../out/usr/include
	make -C libgcc build
	make -C libinit build
	make -C libssp build
	make -C libsys build

clean:
	make -C libgcc clean
	make -C libinit clean
	make -C libssp clean
	make -C libsys clean
