build:
	cp -r include/* ../out/system/include
	make -C libgcc build
	make -C libinit build
	make -C libssp build
	make -C libsys build
	make -C libgraph build

clean:
	make -C libgcc clean
	make -C libinit clean
	make -C libssp clean
	make -C libsys clean
	make -C libgraph clean
