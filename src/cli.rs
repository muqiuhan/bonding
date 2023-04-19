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

use std::path::PathBuf;
use structopt::StructOpt;

use crate::error::ErrorCode;

#[derive(Debug, StructOpt)]
#[structopt(name = "bonding", about = "crude container")]
pub struct Args {
    /// Whether to enable Debug mode
    #[structopt(short, long)]
    debug: bool,

    /// The command with arguments to be executed inside the container
    #[structopt(short, long)]
    pub command: String,

    /// The uid that will be created
    #[structopt(short, long)]
    pub uid: u32,

    /// An external folder inside the container as root
    #[structopt(parse(from_os_str), short, long)]
    pub mount: PathBuf,
}

pub fn parse_args() -> Result<Args, ErrorCode> {
    let args = Args::from_args();

    if !args.mount.exists() || !args.mount.is_dir() {
        return Err(ErrorCode::ArgumentInvalid("mount"));
    }

    Ok(args)
}
