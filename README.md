<div align="center">

<img src="./.github/logo.png" height="150px" width="150px">

# Bonding

*A Minimalism container implementation in C++*

</div>

## Dependencies
- [spdlog (MIT): Fast C++ logging library.](https://github.com/gabime/spdlog)
- [result (MIT): Result<T, E> for Modern C++](https://github.com/p-ranav/result)
- [structopt (MIT): Parse command line arguments by defining a struct](https://github.com/p-ranav/structopt)
- [libseccomp (LGPL): The main libseccomp repository](https://github.com/seccomp/libseccomp)
  > For most Linux distributions, it can be installed through package management, (the package name is like `libseccomp-dev`)
- [libponci (LGPL): Poor mans cgroup interface](https://github.com/caps-tum/libponci)
  
## Build And Run
> [xmake](https://xmake.io) is a cross-platform build utility based on Lua.

Download with `git clone https://github.com/muqiuhan/bonding --recurse-submodules`

### build
__`xmake f -m release && xmake build` or build for debug: `xmake f -m debug && xmake build`__

#### NOTE
- libseccomp may have `seccomp-syscalls.h: No such file or directory` problems, you need to manually change `#include <seccomp-syscalls.h>` in `seccomp.h` to `#include ”seccomp -syscalls.h"`.

### run
__`xmake run -- bonding`__
  
### Debug

E.g:
```
sudo lldb ./build/linux/x86_64/debug/bonding -- \
    --command "sleep 3" \
    --uid 0 \
    --mount_dir . \
    --hostname Test \
    --debug \
```

## USAGE:

```
USAGE: bonding [FLAGS] [OPTIONS] 

FLAGS:
    -d, --debug

OPTIONS:
    -c, --command <command>
    -u, --uid <uid>
    -m, --mount-dir <mount_dir>
    -h, --hostname <hostname>
    --help <help>
    -v, --version <version>
```

e.g:
```
xmake run -- bonding --debug
    --command "/bin/ls -lh"
    --uid 0
    --mount_dir .
    --hostname Test
```

or run in debugger:
```
xmake r -d -- bonding --debug
    --command "/bin/ls -lh"
    --uid 0
    --mount_dir .
    --hostname Test
```

## NOTE

The default syscalls will refuse in container are:

- Kernel keyring
  - keyctl
  - add_key
  - request_key
- NUMA (memory management)
  - mbind
  - migrate_pages
  - move_pages
  - set_mempoliyc
- Allow userland to handle memory faults in the kernel
  - userfaultfd
- Trace / profile syscalls
  - perf_event_open

## REFERENCES

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
- [Linux containers in 500 lines of code](https://blog.lizzie.io/linux-containers-in-500-loc.html)
- [Writing a container in Rust](https://litchipi.github.io/series/container_in_rust)

# LICENSE
Copyright (C) 2022 Muqiu Han

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
