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

use nix::sys::socket::{recv, send, socketpair, AddressFamily, MsgFlags, SockFlag, SockType};
use std::os::unix::io::RawFd;

pub fn send_boolean(fd: RawFd, boolean: bool) -> Result<(), ErrorCode> {
    let data: [u8; 1] = [boolean.into()];
    if let Err(e) = send(fd, &data, MsgFlags::empty()) {
        error!("cannot send boolean through socket: {:?}", e);
        return Err(ErrorCode::SocketError(1));
    };
    Ok(())
}

pub fn recv_boolean(fd: RawFd) -> Result<bool, ErrorCode> {
    let mut data: [u8; 1] = [0];
    if let Err(e) = recv(fd, &mut data, MsgFlags::empty()) {
        error!("Cannot receive boolean from socket: {:?}", e);
        return Err(ErrorCode::SocketError(2));
    }
    Ok(data[0] == 1)
}

pub fn generate_socketpair() -> Result<(RawFd, RawFd), ErrorCode> {
    match socketpair(
        AddressFamily::Unix,
        // Use communication semantics with packets and fixed-length packets
        SockType::SeqPacket,
        // Use the default protocol associated with this socket type.
        None,
        // After any exec-series syscall, the socket will be automatically closed
        SockFlag::SOCK_CLOEXEC,
    ) {
        Ok(res) => Ok(res),
        Err(_) => Err(ErrorCode::SocketError(0)),
    }
}