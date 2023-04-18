/// The MIT License (MIT)
///
/// Copyright (c) 2022 Muqiu Han
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
use crate::cli::Args;
use crate::config::ContainerConfig;
use crate::error::ErrorCode;

pub struct Container {
    config: ContainerConfig,
}

impl Container {
    pub fn new(args: Args) -> Result<Container, ErrorCode> {
        let config = ContainerConfig::new(args.command, args.uid, args.mount)?;
        Ok(Container { config })
    }

    pub fn create(&mut self) -> Result<(), ErrorCode> {
        info!("container has been created");
        Ok(())
    }

    /// This function will be called every time before the container exits
    pub fn clean_exit(&mut self) -> Result<(), ErrorCode> {
        info!("container has been cleaned");
        Ok(())
    }
}

/// Handles everything from container creation to exit
pub fn start(args: Args) -> Result<(), ErrorCode> {
    let mut container = Container::new(args)?;
    if let Err(e) = container.create() {
        container.clean_exit()?;
        log::error!("error while creating container: {:?}", e);
        return Err(e);
    }
    log::info!("finished, cleaning & exit");
    container.clean_exit()
}
