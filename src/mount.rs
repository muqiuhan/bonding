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
use nix::{
    mount::MsFlags,
    unistd::{chdir, pivot_root},
};
use std::{
    fs::create_dir_all,
    path::PathBuf,
    time::{SystemTime, UNIX_EPOCH},
};

pub mod wrapper {
    use std::{fs::remove_dir, path::PathBuf};

    use nix::mount::{mount, umount2, MntFlags, MsFlags};

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
                error!("unable to umount {}: {}", path.to_str().unwrap(), e);
                Err(ErrorCode::MountsError(0))
            }
        }
    }

    pub fn delete_dir(path: &PathBuf) -> Result<(), ErrorCode> {
        match remove_dir(path.as_path()) {
            Ok(_) => Ok(()),
            Err(e) => {
                log::error!(
                    "unable to delete directory {}: {}",
                    path.to_str().unwrap(),
                    e
                );
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

fn mount_user_given_dir_to_temp_dir(mount_dir: &PathBuf) -> Result<PathBuf, ErrorCode> {
    let new_root = PathBuf::from(format!("/tmp/bonding_{}", random_string(10)));
    info!(
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

fn root_pivot(new_root: &PathBuf) -> Result<PathBuf, ErrorCode> {
    info!("privoting root...");
    let old_root_tail = PathBuf::from(format!("oldroot_{}", random_string(10)));
    let put_old = new_root.join(&old_root_tail);
    create_directory(&put_old)?;

    if let Err(_) = pivot_root(new_root, &put_old) {
        return Err(ErrorCode::MountsError(4));
    }

    Ok(old_root_tail.clone())
}

fn clean_unnecessary_dir(old_root_tail: &PathBuf) -> Result<(), ErrorCode> {
    info!("unmounting old root...");
    let old_root = &PathBuf::from(format!("/{}", old_root_tail.to_str().unwrap()));
    if let Err(_) = chdir(&PathBuf::from("/")) {
        return Err(ErrorCode::MountsError(5));
    }

    wrapper::umount_dir(&old_root)?;
    wrapper::delete_dir(&old_root)?;

    Ok(())
}

pub fn set_mount_point(
    mount_dir: &PathBuf,
    addpaths: &Vec<(PathBuf, PathBuf)>,
) -> Result<(), ErrorCode> {
    mount_system_root()?;

    let new_root = mount_user_given_dir_to_temp_dir(mount_dir)?;
    let old_root_tail = root_pivot(&new_root)?;

    clean_unnecessary_dir(&old_root_tail)?;

    info!("mounting additional paths...");
    for (inpath, mntpath) in addpaths.iter() {
        let outpath = new_root.join(mntpath);
        create_directory(&outpath)?;
        wrapper::mount_dir(
            Some(inpath),
            &outpath,
            vec![MsFlags::MS_PRIVATE, MsFlags::MS_BIND],
        )?;
    }

    info!("privotion root");

    Ok(())
}

pub fn clean_mounts(root: &PathBuf) -> Result<(), ErrorCode> {
    Ok(())
}
