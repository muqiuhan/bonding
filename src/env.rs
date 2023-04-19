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
