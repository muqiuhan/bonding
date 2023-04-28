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

use std::fs::{canonicalize, remove_dir};

use cgroups_rs::{cgroup_builder::CgroupBuilder, hierarchies::V2, CgroupPid, MaxValue};
use nix::unistd::Pid;
use rlimit::{setrlimit, Resource};

use crate::error::ErrorCode;

static KMEM_LIMIT: i64 = 1024 * 1024 * 1024;
static MEM_LIMIT: i64 = KMEM_LIMIT;
static MAX_PID: MaxValue = MaxValue::Value(64);
static NOFILE_RLIMIT: u64 = 64;

pub fn restrict_resources(hostname: &String, pid: &Pid) -> Result<(), ErrorCode> {
    info!("restricting resources for hostname {}", hostname);

    let cgs = CgroupBuilder::new(hostname)
        .cpu()
        .shares(256)
        .done()
        .memory()
        .kernel_memory_limit(KMEM_LIMIT)
        .memory_hard_limit(MEM_LIMIT)
        .done()
        .pid()
        .maximum_number_of_processes(MAX_PID)
        .done()
        .blkio()
        .weight(50)
        .done()
        .build(Box::new(V2::new()));

    let pid: u64 = pid.as_raw().try_into().unwrap();
    if let Err(_) = cgs.add_task(CgroupPid::from(pid)) {
        return Err(ErrorCode::ResourceError(0));
    };

    if let Err(_) = setrlimit(Resource::NOFILE, NOFILE_RLIMIT, NOFILE_RLIMIT) {
        return Err(ErrorCode::ResourceError(0));
    }

    Ok(())
}

pub fn clean_cgroups(hostname: &String) -> Result<(), ErrorCode> {
    info!("cleaning cgroups...");
    match canonicalize(format!("/sys/fs/cgroup/{}/", hostname)) {
        Ok(d) => {
            if let Err(_) = remove_dir(d) {
                return Err(ErrorCode::ResourceError(2));
            }
        }
        Err(e) => {
            error!("error while canonicalize path: {}", e);
            return Err(ErrorCode::ResourceError(3));
        }
    }
    Ok(())
}
