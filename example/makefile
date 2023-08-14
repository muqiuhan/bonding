config:
	mkdir ./mount_dir/bin
	cp /usr/bin/bash ./mount_dir/bin
	gcc mount_dir/test.c -o mount_dir/test

test:
	sudo lldb .././build/linux/x86_64/debug/bonding -- --config="config.toml"