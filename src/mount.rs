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
use nix::{mount::MsFlags, unistd::{pivot_root, chdir}};
use std::{
    fs::create_dir_all,
    path::PathBuf,
    time::{SystemTime, UNIX_EPOCH},
};

mod wrapper {
    use std::{path::PathBuf, fs::remove_dir};

    use nix::{
        mount::{mount, MsFlags, MntFlags, umount2},
    };

    use crate::error::ErrorCode;

    pub fn mount_dir(
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

    pub fn umount_dir(path: &PathBuf) -> Result<(), ErrorCode> {
        match umount2(path, MntFlags::MNT_DETACH) {
            Ok(_) => Ok(()),
            Err(e) => {
                error!("unable to umount {}: {}", path.to_str().unwrap(), e)
                Err(ErrorCode::MountsError(0))
            }
        }
    }

    pub fn delete_dir(path: &PathBuf) -> Result<(), ErrorCode>{
        match remove_dir(path.as_path()){
            Ok(_) => Ok(()),
            Err(e) => {
                log::error!("unable to delete directory {}: {}", path.to_str().unwrap(), e);
                Err(ErrorCode::MountsError(1))
            }
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

fn mount_system_root() -> Result<(), ErrorCode> {
    info!("setting mount point...");

    wrapper::mount_dir(
        None,
        &PathBuf::from("/"),
        vec![MsFlags::MS_PRIVATE, MsFlags::MS_REC],
    )?;

    Ok(())
}

fn mount_user_given_dir_to_temp_dir(mount_dir: &PathBuf) ->  Result<PathBuf, ErrorCode> {
    let new_root = PathBuf::from(format!("/tmp/bonding_{}", random_string(10)));
    debug!(
        "mounting temp directory {}",
        new_root.as_path().to_str().unwrap()
    );
    create_directory(&new_root)?;
    wrapper::mount_dir(
        Some(&mount_dir),
        &new_root,
        vec![MsFlags::MS_BIND, MsFlags::MS_PRIVATE],
    )?;
    Ok(new_root.clone())
 }

 fn root_pivot(new_root: &PathBuf) -> Result<PathBuf, ErrorCode>{
    debug!("privoting root!");
    let old_root_tail = PathBuf::from(format!("oldroot_{}", random_string(10)));
    let put_old = new_root.join(old_root_tail);
    create_directory(&put_old)?;

    if let Err(_) = pivot_root(new_root, &put_old) {
        return Err(ErrorCode::MountsError(4));
    }

    Ok(old_root_tail)
 }

 fn clean_unnecessary_dir(old_root_tail: &PathBuf) -> Result<(), ErrorCode>{
    debug!("unmounting old root...");
    let old_root = &PathBuf::from(format!("/{}", old_root_tail.to_str().unwrap()));
    if let Err(_) = chdir(&PathBuf::from("/")) {
        return Err(ErrorCode::MountsError(5));
    }

    wrapper::umount_dir(&old_root)?;
    wrapper::delete_dir(&old_root)?;

    Ok(())
 }

pub fn set_mount_point(mount_dir: &PathBuf) -> Result<(), ErrorCode> {
    mount_system_root()?;

    let new_root = mount_user_given_dir_to_temp_dir(mount_dir)?;
    let old_root_tail = root_pivot(&new_root)?;

    clean_unnecessary_dir(&old_root_tail)?;

    Ok(())
}

pub fn clean_mounts(root: &PathBuf) -> Result<(), ErrorCode> {
    Ok(())
}
