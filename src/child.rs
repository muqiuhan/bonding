/*! Copyright (C) 2022 Muqiu Han
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>. */

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
    set_mount_point(&config.mount_dir, &config.add_paths)?;

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
