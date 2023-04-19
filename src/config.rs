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
}

impl ContainerConfig {
    pub fn new(
        command: String,
        uid: u32,
        mount_dir: PathBuf,
    ) -> Result<(ContainerConfig, (RawFd, RawFd)), ErrorCode> {
        let argv: Vec<CString> = command
            .split_ascii_whitespace()
            .map(|s| CString::new(s).expect("Cannot read arg"))
            .collect();
        let path = argv[0].clone();
        let sockets = generate_socketpair()?;

        Ok((
            ContainerConfig {
                path,
                argv,
                uid,
                mount_dir,
                fd: sockets.1.clone(),
            },
            sockets,
        ))
    }
}
