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
