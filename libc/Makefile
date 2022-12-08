build:
	cp -r include/* ../out/system/include
	make --quiet -C runtime build
	make --quiet -C src build

clean:
	make -C runtime clean
	make -C src clean
