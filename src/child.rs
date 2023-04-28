/*! The MIT License (MIT)
 **
 ** Copyright (c) 2022 Muqiu Han
 **
 ** Permission is hereby granted, free of charge, to any person obtaining a copy
 ** of this software and associated documentation files (the "Software"), to deal
 ** in the Software without restriction, including without limitation the rights
 ** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 ** copies of the Software, and to permit persons to whom the Software is
 ** furnished to do so, subject to the following conditions:
 **
 ** The above copyright notice and this permission notice shall be included in all
 ** copies or substantial portions of the Software.
 **
 ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 ** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 ** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 ** SOFTWARE.
 **/

use std::path::PathBuf;

use crate::config::ContainerConfig;
use crate::error::ErrorCode;

use crate::hostname::set_container_hostname;
use crate::mount;
use crate::mount::set_mount_point;
use crate::syscall::setsyscalls;
use nix::mount::MsFlags;
use nix::sched::clone;
use nix::sched::CloneFlags;
use nix::sys::signal::Signal;
use nix::unistd::execve;
use nix::unistd::Pid;
use std::ffi::CString;

const STACK_SIZE: usize = 1024 * 1024;

fn child(config: ContainerConfig) -> isize {
    info!(
        "starting container with command {} and args {:?}",
        config.path.to_str().unwrap(),
        config.argv
    );

    let retcode = match execve::<CString, CString>(&config.path, &config.argv, &[]) {
        Ok(_) => 0,
        Err(e) => {
            error!("error while trying to perform execve: {:?}", e);
            -1
        }
    };

    match setup_container_configurations(&config) {
        Ok(_) => info!("container setup successfully"),
        Err(e) => {
            error!("error while setup container: {:?}", e);
            return -1;
        }
    }

    retcode
}

fn setup_container_configurations(config: &ContainerConfig) -> Result<(), ErrorCode> {
    set_container_hostname(&config.hostname)?;
    set_mount_point(&config.mount_dir)?;

    mount::wrapper::mount_dir(
        None,
        &PathBuf::from("/"),
        vec![MsFlags::MS_REC, MsFlags::MS_PRIVATE],
    )?;

    setsyscalls()?;
    Ok(())
}

fn child_flags() -> CloneFlags {
    let mut flags = CloneFlags::empty();

    // Start the cloned subroutine in a new mounted namespace, initialized with a copy of the parent process's namespace.
    flags.insert(CloneFlags::CLONE_NEWNS);

    // Start the cloned child process in a new cgroup namespace.
    flags.insert(CloneFlags::CLONE_NEWCGROUP);

    // Start the cloned child process in a new pid namespace.
    flags.insert(CloneFlags::CLONE_NEWPID);

    // Start the cloned child process in a new ipc namespace.
    flags.insert(CloneFlags::CLONE_NEWIPC);

    // Start the cloned subroutine in a new network namespace.
    flags.insert(CloneFlags::CLONE_NEWNET);

    // Start the cloned child process in a new uts namespace.
    flags.insert(CloneFlags::CLONE_NEWUTS);

    flags
}

pub fn generate_child_process(config: ContainerConfig) -> Result<Pid, ErrorCode> {
    let mut tmp_stack: [u8; STACK_SIZE] = [0; STACK_SIZE];

    match clone(
        Box::new(|| child(config.clone())),
        &mut tmp_stack,
        child_flags(),
        Some(Signal::SIGCHLD as i32),
    ) {
        Ok(pid) => Ok(pid),
        Err(_) => Err(ErrorCode::ChildProcessError(0)),
    }
}
