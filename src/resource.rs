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
    if let Err(_) = cgs.unwrap().add_task(CgroupPid::from(pid)) {
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
