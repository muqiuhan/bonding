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

use crate::error::ErrorCode;
use crate::process::generate_socketpair;

use std::ffi::CString;
use std::os::fd::RawFd;
use std::path::PathBuf;

#[derive(Clone)]
pub struct ContainerConfig {
    pub argv: Vec<CString>,

    // Path to the binary/executable/script to be executed inside the container
    pub path: CString,

    // The ID of the user within the container.
    // ID = 0 means it is root
    pub uid: u32,

    // The directory path to use as /root in the container.
    pub mount_dir: PathBuf,

    pub fd: RawFd,

    pub hostname: String,

    pub add_paths: Vec<(PathBuf, PathBuf)>,
}

impl ContainerConfig {
    pub fn new(
        command: String,
        uid: u32,
        mount_dir: PathBuf,
        hostname: String,
        add_paths: Vec<(PathBuf, PathBuf)>,
    ) -> Result<(ContainerConfig, (RawFd, RawFd)), ErrorCode> {
        let argv: Vec<CString> = command
            .split_ascii_whitespace()
            .map(|s| CString::new(s).expect("cannot read arg"))
            .collect();
        let path = argv[0].clone();
        let sockets = generate_socketpair()?;

        Ok((
            ContainerConfig {
                path,
                argv,
                uid,
                mount_dir,
                hostname,
                add_paths,
                fd: sockets.1.clone(),
            },
            sockets,
        ))
    }
}
