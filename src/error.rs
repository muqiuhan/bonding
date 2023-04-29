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
