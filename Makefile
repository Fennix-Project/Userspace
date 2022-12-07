build:
	mkdir -p out
	mkdir -p out/system
	mkdir -p out/system/lib
	mkdir -p out/system/include
	make --quiet -C libc build
	make --quiet -C libs build
	make --quiet -C apps build

prepare:
	$(info Nothing to prepare)

clean:
	rm -rf out
	make -C libc clean
	make -C libs clean
	make -C apps clean
