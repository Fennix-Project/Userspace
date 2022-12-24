build:
	cp include/* ../out/system/include
	make --quiet -C libgcc build
	make --quiet -C libinit build
	make --quiet -C libssp build
	make --quiet -C libsys build
	make --quiet -C libgraph build

clean:
	make -C libgcc clean
	make -C libinit clean
	make -C libssp clean
	make -C libsys clean
	make -C libgraph clean
