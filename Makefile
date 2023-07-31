fmt: 
	find src -iname '*.h' -o -iname '*.cpp' -o -iname '*.hpp' | xargs clang-format -i

build:
	xmake f -m debug && xmake build

release:
	xmake f -m release && build

debug:
	sudo lldb ./build/linux/x86_64/debug/bonding --  --command "/bin/ls -lh > test.log" --uid 0 --mount_dir . --debug