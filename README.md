<div align="center">

<img src="./.github/logo.png" height="150px" width="150px">

# Bonding

> A Minimalism container implementation in C++

</div>

## Build And Run
> [xmake](https://xmake.io) is a cross-platform build utility based on Lua.

- build: `xmake build --release`
- run: `xmake run -- bonding`
- debug: `xmake run -d -- bonding`

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

## REFERENCES

The following are related terms used when reading the above two articles, extended explanations/references of concepts:

- Book: *The Linux Programming Interface*
- OS-level virtualization wikipeidia : [https://en.wikipedia.org/wiki/OS-level_virtualization](https://en.wikipedia.org/wiki/OS-level_virtualization)
- LXC is a userspace interface for the Linux kernel containment features : [https://linuxcontainers.org/lxc/introduction/](https://linuxcontainers.org/lxc/introduction/)
- Advanced Bash-Scripting Guide - Appendix E. Exit Codes With Special Meanings : [https://tldp.org/LDP/abs/html/exitcodes.html](https://tldp.org/LDP/abs/html/exitcodes.html)
- The Rust Programming Language : [https://doc.rust-lang.org/book/title-page.html](https://doc.rust-lang.org/book/title-page.html)
- Linux man pages online : [https://man7.org/linux/man-pages/index.html](https://man7.org/linux/man-pages/index.html)
- Inter-process communication in Linux: Sockets and signals : [https://opensource.com/article/19/4/interprocess-communication-linux-networking](https://opensource.com/article/19/4/interprocess-communication-linux-networking)
- Linux namespaces wikipeidia : [https://en.wikipedia.org/wiki/Linux_namespaces](https://en.wikipedia.org/wiki/Linux_namespaces)
- CAP_SYS_ADMIN: the new root : [https://lwn.net/Articles/486306/](https://lwn.net/Articles/486306/)
- Mount namespaces and shared subtrees : [https://lwn.net/Articles/689856/](https://lwn.net/Articles/689856/)
- Mount namespaces, mount propagation, and unbindable mounts : [https://lwn.net/Articles/690679/](https://lwn.net/Articles/690679/)
- Linux user namespaces might not be secure enough? a.k.a. subverting POSIX capabilities : [https://medium.com/@ewindisch/linux-user-namespaces-might-not-be-secure-enough-a-k-a-subverting-posix-capabilities-f1c4ae19cad](https://medium.com/@ewindisch/linux-user-namespaces-might-not-be-secure-enough-a-k-a-subverting-posix-capabilities-f1c4ae19cad)
- Linux File Permission: uid vs gid : [https://www.cbtnuggets.com/blog/technology/system-admin/linux-file-permission-uid-vs-gid](https://www.cbtnuggets.com/blog/technology/system-admin/linux-file-permission-uid-vs-gid)
- Difference between Real User ID, Effective User ID and Saved User ID : [https://stackoverflow.com/questions/32455684/difference-between-real-user-id-effective-user-id-and-saved-user-id/32456814#32456814](https://stackoverflow.com/questions/32455684/difference-between-real-user-id-effective-user-id-and-saved-user-id/32456814#32456814)
- Introduction to Trusted Execution Environment: ARM's TrustZone : [https://blog.quarkslab.com/introduction-to-trusted-execution-environment-arms-trustzone.html](https://blog.quarkslab.com/introduction-to-trusted-execution-environment-arms-trustzone.html)
- eMMC transfer state.jssm : [https://gist.github.com/StoneCypher/be7f117881915e7df7bbc96c5c0a84d5](https://gist.github.com/StoneCypher/be7f117881915e7df7bbc96c5c0a84d5)
- List of Linux Syscalls : [https://linuxhint.com/list_of_linux_syscalls/](https://linuxhint.com/list_of_linux_syscalls/)
- seccomp wikipeidia : [https://en.wikipedia.org/wiki/Seccomp](https://en.wikipedia.org/wiki/Seccomp)
- seccomp docker doc : [https://github.com/docker/docs/blob/main/engine/security/seccomp.md](https://github.com/docker/docs/blob/main/engine/security/seccomp.md)
- Control groups series by Neil Brown : [https://lwn.net/Articles/604609/](https://lwn.net/Articles/604609/)
- New Linux Kernel cgroups Vulnerability Could Let Attackers Escape Container : [https://thehackernews.com/2022/03/new-linux-kernel-cgroups-vulnerability.html](https://thehackernews.com/2022/03/new-linux-kernel-cgroups-vulnerability.html)
- How to manage cgroups with CPUShares : [https://www.redhat.com/sysadmin/cgroups-part-two](https://www.redhat.com/sysadmin/cgroups-part-two)
- Limits on resources in Linux : [https://0xax.gitbooks.io/linux-insides/content/SysCall/linux-syscall-6.html](https://0xax.gitbooks.io/linux-insides/content/SysCall/linux-syscall-6.html)
- Linux containers in 500 lines of code : [https://blog.lizzie.io/linux-containers-in-500-loc.html](https://blog.lizzie.io/linux-containers-in-500-loc.html)
- Writing a container in Rust : [https://litchipi.github.io/series/container_in_rust](https://litchipi.github.io/series/container_in_rust)

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