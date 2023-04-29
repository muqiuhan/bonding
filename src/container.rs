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
