<div align="center">

<img src="./.github/logo.png" height="150px" width="150px">

# Bonding

> A simple and crude container implementation in Rust

</div>

# MOTIVATION
Mainly to study Docker, accompanied by research on the working principles of Linux security measures, containers, virtualization and other features, and to learn Rust.

So this is just a trial project for learning and use, please do not use it in any production development environment (`^_^`)

# Build And Run
Bonding can only be used for Linux, and it is also developed on Linux. The Rust version used is: `rustc 1.70.0-nightly (af06dce64 2023-04-08)`

- build: `cargo build --release`
- run: `$ sudo ./target/debug/bonding --help`

USAGE:
```
bonding 0.1.0
A simple and crude container implementation in Rust

USAGE:
    bonding [FLAGS] [OPTIONS]
        --command <command>
        --hostname <hostname>
        --mount-dir <mount>
        --uid <uid>

FLAGS:
        --debug      Whether to enable Debug mode
    -h, --help       Prints help information
    -V, --version    Prints version information

OPTIONS:
        --add <addpaths>...      Mount a dir inside the container
        --command <command>      The command with arguments to be executed inside the container
        --hostname <hostname>    The hostname
        --mount-dir <mount>      An external folder inside the container as root
        --uid <uid>              The uid that will be created
```

E.g:
```
$ sudo ./target/release/bonding --command "/bin/ls -lh" \
                                --hostname MuqiuHan-0   \
                                --uid 0                 \
                                --mount-dir .           \
```

# ACKNOWLEDGMENT

These two articles are my main source of inspiration:

- Linux containers in 500 lines of code : [https://blog.lizzie.io/linux-containers-in-500-loc.html](https://blog.lizzie.io/linux-containers-in-500-loc.html)
- Writing a container in Rust : [https://litchipi.github.io/series/container_in_rust](https://litchipi.github.io/series/container_in_rust)

# REFERENCES

The following are related terms used when reading the above two articles, extended explanations/references of concepts:

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

# LICENSE
> See full license : [LICENSE](./LICENSE)

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