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

use std::process::exit;

use crate::error::exit_with_code;
use colog;
use log::LevelFilter;
use std::io::{self, Write};

#[macro_use]
extern crate scan_fmt;

mod child;
mod cli;
mod config;
mod container;
mod env;
mod error;
mod hostname;
mod mount;
mod process;
mod resource;
mod syscall;

#[macro_use]
extern crate log;

fn main() {
    let mut logger = colog::builder();
    logger.filter(None, LevelFilter::Debug);
    logger.init();

    match cli::parse_args() {
        Ok(args) => {
            debug!("{:?}", args);
            exit_with_code(container::start(args))
        }
        Err(e) => {
            error!("error while parsing arguments:\n\t{}", e);
            exit(e.get_retcode());
        }
    }
    io::stdout().flush().unwrap();
}
