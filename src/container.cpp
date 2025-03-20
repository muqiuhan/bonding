/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/container.h"
#include "include/config.h"
#include "include/ipc.h"
#include "include/namespace.h"
#include "include/resource.h"
#include "include/syscall.h"
#include "include/unix.h"
#include <error.h>

namespace bonding::container
{
  std::expected<void, error::Err> Container::create() noexcept
  {
    if (ipc::IPC::recv_boolean(m_sockets.first))
      {
        ns::Namespace::handle_child_uid_map(m_child_process.m_pid).value();
        resource::Resource::setup(m_config).value();
        ipc::IPC::send_boolean(m_sockets.first, false).value();
      }
    else
      {
        return std::unexpected(
          ERR_MSG(error::Code::Namespace, "No user namespace set up from child process"));
      }
    return m_child_process.wait();
  }

  std::expected<void, error::Err> Container::clean_and_exit() noexcept
  {
    Container_Cleaner::close_socket(m_sockets.first).value();
    Container_Cleaner::close_socket(m_sockets.second).value();
    resource::Resource::clean(m_config).value();
    syscall::Syscall::Syscall::clean().value();

    return {};
  }

  std::expected<void, error::Err>
    Container::start(const config::Container_Options & argv) noexcept
  {
    Container container(argv);

    if (argv.debug)
      {
        log::set_level(LOG_LEVEL_DEBUG);
        LOG_DEBUG << "Activate debug mode...✓";
      }

    return container.create()
      .transform([&]() {
      LOG_INFO << "Cleaning and exiting container...✓";
      return container.clean_and_exit().value();
    }).transform_error([&](const error::Err e) {
      container.clean_and_exit().value();
      return ERR_MSG(
        error::Code::Container, "Error while creating container: {}" + e.to_string());
    });
  }

  std::expected<void, error::Err>
    Container_Cleaner::close_socket(const int socket) noexcept
  {
    unix::Filesystem::Close(socket)
      .transform_error([&](const auto & e) {
      return ERR_MSG(
        error::Code::Socket, "Unable to close socket " + std::to_string(socket));
    }).value();

    LOG_DEBUG << "Closing socket " << socket << "...✓";
  }
} // namespace bonding::container
