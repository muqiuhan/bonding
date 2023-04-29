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
use nix::sys::utsname::{uname, UtsName};

pub const MINIMAL_KERNEL_VERSION: f32 = 4.8;

unsafe fn get_uname() -> UtsName {
    match uname() {
        Ok(utsname) => utsname,
        Err(e) => {
            error!("{}", e.to_string());
            panic!("{}", e.to_string())
        }
    }
}

fn get_release(uname: UtsName) -> Result<String, ErrorCode> {
    match uname.release().to_str() {
        Some(release) => {
            if let Ok(version) = scan_fmt!(release, "{f}.{}", f32) {
                if version < MINIMAL_KERNEL_VERSION {
                    return Err(ErrorCode::NotSupported(0));
                } else {
                    Ok(String::from(release))
                }
            } else {
                return Err(ErrorCode::ContainerError(0));
            }
        }

        None => {
            error!("unable to get the kernel version of the operating system!");
            return Err(ErrorCode::ContainerError(0));
        }
    }
}

fn get_machine(uname: UtsName) -> Result<String, ErrorCode> {
    match uname.machine().to_str() {
        Some(machine) => {
            if machine != "x86_64" {
                return Err(ErrorCode::NotSupported(1));
            } else {
                Ok(String::from(machine))
            }
        }
        None => Err(ErrorCode::ContainerError(0)),
    }
}

pub fn check_linux_version() -> Result<(), ErrorCode> {
    let uname = unsafe { get_uname() };
    let release = get_release(uname)?;
    let machine = get_machine(uname)?;

    info!("Linux release: {} on {}", release, machine);

    Ok(())
}
