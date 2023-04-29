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
use libc::TIOCSTI;
use nix::sched::CloneFlags;
use nix::sys::stat::Mode;
use syscallz::{Action, Cmp, Comparator, Context, Syscall};

static EPERM: u16 = 1;

static S_ISUID: u64 = Mode::S_ISUID.bits() as u64;
static S_ISGID: u64 = Mode::S_ISGID.bits() as u64;
static CLONE_NEW_USER: u64 = CloneFlags::CLONE_NEWUSER.bits() as u64;

static SYSCALL_REFUSED: [Syscall; 9] = [
    Syscall::keyctl,
    Syscall::add_key,
    Syscall::request_key,
    Syscall::mbind,
    Syscall::migrate_pages,
    Syscall::move_pages,
    Syscall::set_mempolicy,
    Syscall::userfaultfd,
    Syscall::perf_event_open,
];

static SYSCALL_REFUSE_IFCOMP: [(Syscall, u32, u64); 9] = [
    (Syscall::chmod, 1, S_ISUID),
    (Syscall::chmod, 1, S_ISGID),
    (Syscall::fchmod, 1, S_ISUID),
    (Syscall::fchmod, 1, S_ISGID),
    (Syscall::fchmodat, 2, S_ISUID),
    (Syscall::fchmodat, 2, S_ISGID),
    (Syscall::unshare, 0, CLONE_NEW_USER),
    (Syscall::clone, 0, CLONE_NEW_USER),
    (Syscall::ioctl, 1, TIOCSTI),
];

fn load_ctx(ctx: &Context) -> Result<(), ErrorCode> {
    if let Err(_) = ctx.load() {
        return Err(ErrorCode::SyscallError(0));
    }

    Ok(())
}

fn refuse_syscall(ctx: &mut Context) -> Result<(), ErrorCode> {
    for se in SYSCALL_REFUSED.iter() {
        match ctx.set_action_for_syscall(Action::Errno(EPERM), *se) {
            Ok(_) => Ok(()),
            Err(_) => Err(ErrorCode::SyscallError(2)),
        }?;
    }

    Ok(())
}

fn add_rules(ctx: &mut Context) -> Result<(), ErrorCode> {
    for (sc, ind, biteq) in SYSCALL_REFUSE_IFCOMP.iter() {
        match ctx.set_rule_for_syscall(
            Action::Errno(EPERM),
            *sc,
            &[Comparator::new(*ind, Cmp::MaskedEq, *biteq, Some(*biteq))],
        ) {
            Ok(_) => Ok(()),
            Err(_) => Err(ErrorCode::SyscallError(3)),
        }?;
    }

    Ok(())
}

pub fn setsyscalls() -> Result<(), ErrorCode> {
    info!("refusing / filtering unwanted syscalls");

    if let Ok(mut ctx) = Context::init_with_action(syscallz::Action::Allow) {
        load_ctx(&ctx)?;
        refuse_syscall(&mut ctx)?;
        add_rules(&mut ctx)?;
        Ok(())
    } else {
        Err(ErrorCode::SyscallError(1))
    }
}
