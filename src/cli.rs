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

use std::path::PathBuf;
use structopt::StructOpt;

use crate::error::ErrorCode;

#[derive(Debug, StructOpt)]
#[structopt(
    name = "bonding",
    about = "A simple and crude container implementation in Rust"
)]
pub struct Args {
    /// Mount a dir inside the container
    #[structopt(parse(from_os_str), long = "add")]
    pub addpaths: Vec<PathBuf>,

    /// Whether to enable Debug mode
    #[structopt(long)]
    debug: bool,

    /// The command with arguments to be executed inside the container
    #[structopt(long)]
    pub command: String,

    /// The uid that will be created
    #[structopt(long)]
    pub uid: u32,

    /// The hostname
    #[structopt(long)]
    pub hostname: String,

    /// An external folder inside the container as root
    #[structopt(parse(from_os_str), long = "mount-dir")]
    pub mount: PathBuf,
}

pub fn parse_args() -> Result<Args, ErrorCode> {
    let args = Args::from_args();

    if !args.mount.exists() || !args.mount.is_dir() {
        return Err(ErrorCode::ArgumentInvalid("mount"));
    }

    if args.command.is_empty() {
        return Err(ErrorCode::ArgumentInvalid("command"));
    }

    Ok(args)
}
