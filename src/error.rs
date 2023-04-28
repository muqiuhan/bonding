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

use std::fmt::{self};
use std::process::exit;

#[derive(Debug)]
pub enum ErrorCode {
    ArgumentInvalid(&'static str),
    NotSupported(u8),
    ContainerError(u8),
    SocketError(u8),
    ChildProcessError(u8),
    HostnameError(u8),
    MountsError(u8),
    SyscallError(u8),
    ResourceError(u8),
}

impl fmt::Display for ErrorCode {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match &self {
            Self::ArgumentInvalid(element) => write!(f, "Argument Invalid: {}", element),
            Self::ContainerError(error) => write!(f, "Container Error: {}", error),
            Self::NotSupported(error) => write!(f, "Not Support: {}", error),
            Self::SocketError(error) => write!(f, "Socket Error: {}", error),
            Self::ChildProcessError(error) => write!(f, "Child Process Error: {}", error),
            Self::HostnameError(error) => write!(f, "Hostname Error: {}", error),
            Self::MountsError(error) => write!(f, "Mounts Error: {}", error),
            Self::SyscallError(error) => write!(f, "Syscall Error: {}", error),
            Self::ResourceError(error) => write!(f, "Resource Error: {}", error),
        }
    }
}

pub fn exit_with_code(res: Result<(), ErrorCode>) {
    match res {
        Ok(_) => {
            debug!("Exit without any error, returning 0");
            exit(0);
        }

        Err(e) => {
            let retcode = e.get_retcode();
            error!("Error on exit:\n\t{}\n\tReturning {}", e, retcode);
            exit(retcode);
        }
    }
}

impl ErrorCode {
    pub fn get_retcode(&self) -> i32 {
        match &self {
            _ => 1,
        }
    }
}
