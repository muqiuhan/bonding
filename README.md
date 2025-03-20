<div align="center">

<img src="./.github/logo.png" height="150px" width="150px">

# Bonding

*A Minimalism container implementation in C++*

![](https://github.com/muqiuhan/bonding/actions/workflows/build.yaml/badge.svg) 

![](https://img.shields.io/badge/C++17-123456)
![](https://img.shields.io/badge/Clang16.0.6-00F)
![](https://img.shields.io/badge/GCC13.2.1-00F)

![](https://img.shields.io/badge/WIP-FFFF00)

</div>

## Build from source
> This project is built using [xmake](https://xmake.io).

This is currently the only way to use bonding. Fortunately, xmake supports the generation of files required by build tools such as Ninja, CMake and Makefile, so this project supports building from Ninja or CMakes:

1. clone: `git clone https://github.com/muqiuhan/bonding --recurse-submodules --depth=1`
2. and build it: `make build.release` or build for debug: `make build.debug`
    > or use one of cmake and ninja

## Install

- The easiest is `make install`, it will try to request administrator permission to install

Other install options: 
- `xmake -o INSTALLDIR` to set the install directory.
    >
    > e.g. `xmake install -o /usr/local`
    >
    > or   `DESTDIR=/usr/local xmake install`
    >
    > or   `INSTALLDIR=/usr/local xmake install`

- `xmake -g GROUP` to install  all targets of the given group. It support path pattern matching.
    > e.g.   `xmake install -g test`
    > 
    > or     `xmake install -g test_*`
    > 
    > or     `xmake install --group=benchmark/*`

## Debug
Bonding need advanced permissions to create, such as restricting resources through cgroups, restricting system calls through seccomp, etc. So they must be debug with sudo. For example:

```shell
sudo lldb ./build/linux/x86_64/debug/bonding run --debug
```

## USAGE:
```
Usage: bonding [help] [init] [run] [help] [version]

 [init]
        Initialize the current directory as the container directory

 [run]
        Run with the current directory as the container directory

 [help]
        show this message

 [version]
        show the version of bonding
```

Bonding sets the environment and various parameters through the configuration file [bonding.json](./example/bonding.json):
```json
{
    "hostname": "Test",
    "debug": true,
    "uid": 0,
    "mount_dir": "./mount_dir",
    "command": "/bin/bash",
    "mounts": [
        [
            "/lib",
            "/lib"
        ],
        [
            "/lib64",
            "/lib64"
        ]
    ],
    "clone": [
        "CLONE_NEWNS",
        "CLONE_NEWCGROUP",
        "CLONE_NEWPID",
        "CLONE_NEWIPC",
        "CLONE_NEWNET",
        "CLONE_NEWUTS"
    ],
    "cgroups-v1": {
        "cpu.shares": "256",
        "pids.max": "64",
        "blkio.bfq.weight": "64",
        "memory.limit_in_bytes": "1073741824"
    }
}
```

- `mount_dir` is the root directory where the container runs
- `mounts` is the external directory that the container needs to mount
- `command` is the path and arguments to the application running inside the container
- `clone` is the process running command CLONE_FLAG, see [man clone](https://www.man7.org/linux/man-pages/man2/clone.2.html)
- `cgroups-v1` is used to limit the resources of the container, see [Control Groups Version 1](https://docs.kernel.org/admin-guide/cgroup-v1/index.html)

## Dependencies
- [plog (MIT):  Portable, simple and extensible C++ logging library](https://github.com/SergiusTheBest/plog)
- [cmd_line_parser (MIT):  Command line parser for C++17. ](https://github.com/jermp/cmd_line_parser)
- [libseccomp (LGPL-2.1): The main libseccomp repository](https://github.com/seccomp/libseccomp)
- [nlohmann_json (MIT): JSON for Modern C++](https://github.com/nlohmann/json)

## REFERENCES

- [Linux containers in 500 lines of code](https://blog.lizzie.io/linux-containers-in-500-loc.html)
- Book: *The Linux Programming Interface*
- [The Linux Kernel documentation¶](https://www.kernel.org/doc/html/latest/index.html)
- [OS-level virtualization wikipeidia](https://en.wikipedia.org/wiki/OS-level_virtualization)
- [LXC is a userspace interface for the Linux kernel containment features](https://linuxcontainers.org/lxc/introduction/)
- [Advanced Bash-Scripting Guide - Appendix E. Exit Codes With Special Meanings](https://tldp.org/LDP/abs/html/exitcodes.html)
- [Inter-process communication in Linux: Sockets and signals](https://opensource.com/article/19/4/interprocess-communication-linux-networking)
- [Wikipedia: Linux namespaces](https://en.wikipedia.org/wiki/Linux_namespaces)
- [CAP_SYS_ADMIN: the new root](https://lwn.net/Articles/486306/)
- [Mount namespaces and shared subtrees](https://lwn.net/Articles/689856/)
- [Mount namespaces, mount propagation, and unbindable mounts](https://lwn.net/Articles/690679/)
- [Linux user namespaces might not be secure enough?](https://medium.com/@ewindisch/linux-user-namespaces-might-not-be-secure-enough-a-k-a-subverting-posix-capabilities-f1c4ae19cad)
- [Linux File Permission: uid vs gid](https://www.cbtnuggets.com/blog/technology/system-admin/linux-file-permission-uid-vs-gid)
- [Difference between Real User ID, Effective User ID and Saved User ID](https://stackoverflow.com/questions/32455684/difference-between-real-user-id-effective-user-id-and-saved-user-id/32456814#32456814)
- [Introduction to Trusted Execution Environment: ARM's TrustZone](https://blog.quarkslab.com/introduction-to-trusted-execution-environment-arms-trustzone.html)
- [eMMC transfer state.jssm](https://gist.github.com/StoneCypher/be7f117881915e7df7bbc96c5c0a84d5)
- [List of Linux Syscalls](https://linuxhint.com/list_of_linux_syscalls/)
- [Linux kernel modules drivers](http://www.haifux.org/lectures/86-sil/kernel-modules-drivers/kernel-modules-drivers.html)
- [seccomp wikipeidia](https://en.wikipedia.org/wiki/Seccomp)
- [seccomp docker doc](https://github.com/docker/docs/blob/main/engine/security/seccomp.md)
- [Control groups series](https://lwn.net/Articles/604609/)
- [New Linux Kernel cgroups Vulnerability Could Let Attackers Escape Container](https://thehackernews.com/2022/03/new-linux-kernel-cgroups-vulnerability.html)
- [How to manage cgroups with CPUShares](https://www.redhat.com/sysadmin/cgroups-part-two)
- [Limits on resources in Linux](https://0xax.gitbooks.io/linux-insides/content/SysCall/linux-syscall-6.html)

# LICENSE
Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
