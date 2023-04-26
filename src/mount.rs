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
use nix::{
    mount::{mount, MsFlags},
    unistd::pivot_root,
};
use std::{
    fs::create_dir_all,
    path::PathBuf,
    time::{SystemTime, UNIX_EPOCH},
};

pub fn mount_directory(
    path: Option<&PathBuf>,
    mount_point: &PathBuf,
    flags: Vec<MsFlags>,
) -> Result<(), ErrorCode> {
    let mut ms_flags = MsFlags::empty();
    for f in flags.iter() {
        ms_flags.insert(*f);
    }

    match mount::<PathBuf, PathBuf, PathBuf, PathBuf>(path, mount_point, None, ms_flags, None) {
        Ok(_) => Ok(()),
        Err(e) => {
            if let Some(p) = path {
                error!(
                    "cannot mount {} to {}: {}",
                    p.to_str().unwrap(),
                    mount_point.to_str().unwrap(),
                    e
                );
            } else {
                error!("cannot remount {}: {}", mount_point.to_str().unwrap(), e);
            }
            Err(ErrorCode::MountsError(3))
        }
    }
}

pub fn create_directory(path: &PathBuf) -> Result<(), ErrorCode> {
    match create_dir_all(path) {
        Err(e) => {
            log::error!("cannot create directory {}: {}", path.to_str().unwrap(), e);
            Err(ErrorCode::MountsError(2))
        }
        Ok(_) => Ok(()),
    }
}

fn random_string(n: u8) -> String {
    let mut rng_seed = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_nanos();

    let xorshift = |mut x: u128| -> u128 {
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        x
    };

    let charset: &[u8] = b"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    let mut result = String::new();

    for _ in 0..n {
        rng_seed = xorshift(rng_seed);
        let idx = (rng_seed % charset.len() as u128) as usize;
        result.push(charset[idx] as char);
    }

    result
}

pub fn set_mount_point(mount_dir: &PathBuf) -> Result<(), ErrorCode> {
    info!("setting mount point...");
    mount_directory(
        None,
        &PathBuf::from("/"),
        vec![MsFlags::MS_PRIVATE, MsFlags::MS_REC],
    )?;

    let new_root = PathBuf::from(format!("/tmp/bonding_{}", random_string(10)));
    debug!(
        "mounting temp directory {}",
        new_root.as_path().to_str().unwrap()
    );
    create_directory(&new_root)?;
    mount_directory(
        Some(&mount_dir),
        &new_root,
        vec![MsFlags::MS_BIND, MsFlags::MS_PRIVATE],
    )?;

    debug!("privoting root!");
    let old_root = new_root.join(PathBuf::from(format!("oldroot_{}", random_string(10))));
    create_directory(&old_root)?;

    if let Err(_) = pivot_root(&new_root, &old_root) {
        return Err(ErrorCode::MountsError(4));
    }

    Ok(())
}

pub fn clean_mounts(root: &PathBuf) -> Result<(), ErrorCode> {
    Ok(())
}
