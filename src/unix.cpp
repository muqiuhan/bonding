/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/unix.h"
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <sys/capability.h>
#include <sys/utsname.h>
#include <unistd.h>

namespace bonding::unix
{
  std::expected<void, error::Err> Filesystem::Mkdir(const std::string & path) noexcept
  {
    try
      {
        std::filesystem::create_directories(path);
      }
    catch (...)
      {
        LOG_ERROR << "Cannot create direcotry " << path;
        return std::unexpected(ERR(error::Code::Mounts));
      }

    return {};
  }

  /** Capabilities::get_proc */
  GENERATE_SYSTEM_CALL_WRAPPER(cap_t, nullptr, Capabilities::Get_proc(), cap_get_proc)

  /** Capabilities::set_proc */
  GENERATE_NO_RET_VALUE_SYSTEM_CALL_WRAPPER(
    int, -1, Capabilities::Set_proc(cap_t cap), cap_set_proc, cap)

  /** Capabilities::free */
  GENERATE_NO_RET_VALUE_SYSTEM_CALL_WRAPPER(
    int, -1, Capabilities::Free(cap_t cap), cap_free, cap)

  /** Capabilities::set_flag */
  GENERATE_NO_RET_VALUE_SYSTEM_CALL_WRAPPER(
    int,
    -1,
    Capabilities::Set_flag(
      cap_t               cap,
      cap_flag_t          flag,
      int                 ncap,
      const cap_value_t * caps,
      cap_flag_value_t    value),
    cap_set_flag,
    cap,
    flag,
    ncap,
    caps,
    value)

  /** Filesystem::Close */
  GENERATE_NO_RET_VALUE_SYSTEM_CALL_WRAPPER(int, -1, Filesystem::Close(int fd), close, fd);

  /** Filesystem::Rmdir */
  GENERATE_NO_RET_VALUE_SYSTEM_CALL_WRAPPER(
    int, -1, Filesystem::Rmdir(const std::string & path), rmdir, path.c_str());

  /** Filesystem::Open */
  GENERATE_SYSTEM_CALL_WRAPPER(
    int,
    -1,
    Filesystem::Open(const std::string & file, int flag),
    open,
    file.c_str(),
    flag);

  /** Filesystem::Open */
  GENERATE_SYSTEM_CALL_WRAPPER(
    int,
    -1,
    Filesystem::Open(const std::string & file, int flag, int mode),
    open,
    file.c_str(),
    flag,
    mode);

  /** Filesystem::Write */
  GENERATE_NO_RET_VALUE_SYSTEM_CALL_WRAPPER(
    int,
    -1,
    Filesystem::Write(int fd, const std::string & s),
    write,
    fd,
    s.c_str(),
    s.length());

  std::expected<utsname, error::Err> Utsname::Get() noexcept
  {
    struct utsname host = {0};
    if (-1 == uname(&host))
      return std::unexpected(ERR(error::Code::Unix));

    return host;
  }

  std::expected<std::string, error::Err>
    Filesystem::read_entire_file(const std::string & path) noexcept
  {
    constexpr auto read_size = std::size_t(4096);

    auto stream = std::ifstream(path);
    stream.exceptions(std::ios_base::badbit);

    if (!stream.is_open())
      return std::unexpected(ERR(error::Code::Unix));

    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(&buf[0], read_size))
      out.append(buf, 0, stream.gcount());

    out.append(buf, 0, stream.gcount());
    return out;
  }

  std::expected<void, error::Err>
    Filesystem::Write(const std::string & path, const std::string & s) noexcept
  {
    int fd = Filesystem::Open(path, O_WRONLY | O_CREAT, 0777).value();
    Filesystem::Write(fd, s).value();
    return Filesystem::Close(fd);
  }

} // namespace bonding::unix