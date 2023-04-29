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

use std::os::fd::RawFd;
use std::path::PathBuf;

use nix::sys::wait::waitpid;
use nix::unistd::{close, Pid};

use crate::child::generate_child_process;
use crate::cli::Args;
use crate::config::ContainerConfig;
use crate::env::check_linux_version;
use crate::error::ErrorCode;
use crate::mount::clean_mounts;
use crate::resource::{clean_cgroups, restrict_resources};

pub struct Container {
    config: ContainerConfig,
    sockets: (RawFd, RawFd),
    child_pid: Option<Pid>,
}

impl Container {
    pub fn new(args: Args) -> Result<Container, ErrorCode> {
        let mut addpaths = vec![];
        for ap_pair in args.addpaths.iter() {
            let mut pair = ap_pair.to_str().unwrap().split(":");
            let frompath = PathBuf::from(pair.next().unwrap())
                .canonicalize()
                .expect("Cannot canonicalize path")
                .to_path_buf();
            let mntpath = PathBuf::from(pair.next().unwrap())
                .strip_prefix("/")
                .expect("Cannot strip prefix from path")
                .to_path_buf();
            addpaths.push((frompath, mntpath));
        }

        let (config, sockets) =
            ContainerConfig::new(args.command, args.uid, args.mount, args.hostname, addpaths)?;

        Ok(Container {
            config,
            sockets,
            child_pid: None,
        })
    }

    pub fn create(&mut self) -> Result<(), ErrorCode> {
        let pid = generate_child_process(self.config.clone())?;
        restrict_resources(&self.config.hostname, &pid)?;
        self.child_pid = Some(pid);
        info!("container is created");
        Ok(())
    }

    /// This function will be called every time before the container exits
    pub fn clean_exit(&mut self) -> Result<(), ErrorCode> {
        info!("clean container...");
        if let Err(e) = close(self.sockets.0) {
            error!("unable to close write socket: {:?}", e);
            return Err(ErrorCode::SocketError(3));
        }

        if let Err(e) = close(self.sockets.1) {
            error!("unable to close read socket: {:?}", e);
            return Err(ErrorCode::SocketError(4));
        }

        if let Err(e) = clean_cgroups(&self.config.hostname) {
            error!("cgroups cleaning failed: {}", e);
            return Err(e);
        }

        clean_mounts(&self.config.mount_dir)?;
        Ok(())
    }
}

/// Handles everything from container creation to exit
pub fn start(args: Args) -> Result<(), ErrorCode> {
    check_linux_version()?;
    let mut container = Container::new(args)?;
    if let Err(e) = container.create() {
        container.clean_exit()?;
        error!("error while creating container: {:?}", e);
        return Err(e);
    }
    info!("container child pid: {:?}", container.child_pid);

    wait_child(container.child_pid)?;
    info!("exit container...");
    container.clean_exit()
}

pub fn wait_child(pid: Option<Pid>) -> Result<(), ErrorCode> {
    if let Some(child_pid) = pid {
        info!("waiting for child (pid {}) to finish", child_pid);
        if let Err(e) = waitpid(child_pid, None) {
            error!("error while waiting for pid to finish: {:?}", e);
            return Err(ErrorCode::ContainerError(1));
        }
    }
    Ok(())
}
