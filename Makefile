install:
	xmake f -m release && xmake build && xmake install --installdir=~/.local

uninstall:
	xmake uninstall --installdir=~/.local

build.release:
	xmake f -m release && xmake build

build.debug:
	xmake f -m debug && xmake build

debug:
	xmake f -m debug && xmake build
	cd example && sudo lldb .././build/linux/x86_64/debug/bonding run && cd ..

fmt:
	find src -iname '*.h' -o -iname '*.cpp' -o -iname '*.hpp' | xargs clang-format -i