#include "include/configfile.h"
#include "include/config.h"
#include "include/resource.h"
#include "include/unix.h"
#include <algorithm>
#include <error.h>
#include <exception>
#include <numeric>
#include <stdexcept>
#include <utility>

namespace bonding::configfile
{
  config::Container_Options
  Config_File::read(const std::string & path) noexcept
  {
    const nlohmann::json data =
      nlohmann::json::parse(unix::Filesystem::read_entire_file(path).unwrap());

    return config::Container_Options{ data["debug"],
                                      data["command"],
                                      data["mount_dir"],
                                      data["uid"],
                                      parse_argv(data["command"]).unwrap(),
                                      generate_socketpair().unwrap(),
                                      data["hostname"],
                                      read_mounts(data).unwrap(),
                                      read_clone(data).unwrap(),
                                      read_cgroups_options(data).unwrap() };
  }

  Result<int, error::Err>
  Config_File::read_clone(const nlohmann::json & data) noexcept
  {
    std::vector<int> result;

    try
      {
        for (auto && flag : data["clone"])
          try
            {
              result.push_back(CLONE_FLAGS_MAP.at(flag));
            }
          catch (const std::out_of_range & e)
            {
              return ERR_MSG(error::Code::Configfile,
                             std::string(flag) + " is not a valid flag");
            }
      }
    catch (const nlohmann::json::exception & e)
      {
        return ERR_MSG(error::Code::Configfile, e.what());
      }

    return Ok(
      std::accumulate(result.begin(), result.end(), 0, [](uint32_t a, uint32_t b) {
        return a | b;
      }));
  }

  Result<std::vector<std::pair<std::string, std::string>>, error::Err>
  Config_File::read_mounts(const nlohmann::json & data) noexcept
  {
    std::vector<std::pair<std::string, std::string>> mounts;

    try
      {
        for (auto && [path, mount_point] : data["mounts"].items())
          mounts.push_back(std::make_pair(path, mount_point));
      }
    catch (const nlohmann::json::exception & e)
      {
        return ERR_MSG(error::Code::Configfile, e.what());
      }
    return Ok(mounts);
  }

  Result<std::vector<config::CgroupsV1::Control>, error::Err>
  Config_File::read_cgroups_options(const nlohmann::json & data) noexcept
  {
    std::vector<config::CgroupsV1::Control> options;
    try
      {
        std::string controller;
        std::vector<config::CgroupsV1::Control::Setting> settings;
        for (auto && [setting_name, setting_value] : data["cgroups-v1"].items())
          {
            const std::string current_controller =
              setting_name.substr(0, setting_name.find_first_of("."));

            if (controller != current_controller && (!controller.empty()))
              {
                options.push_back(config::CgroupsV1::Control{ controller, settings });
                settings.clear();
              }

            settings.push_back(
              config::CgroupsV1::Control::Setting{ setting_name, setting_value });

            controller = current_controller;
          }
      }
    catch (const nlohmann::json::exception & e)
      {
        return ERR_MSG(error::Code::Configfile, e.what());
      }
    return Ok(options);
  }

  Result<std::vector<std::string>, Void>
  Config_File::parse_argv(const std::string argv) noexcept
  {
    std::istringstream argv_stream(argv);
    const auto result =
      std::vector<std::string>((std::istream_iterator<std::string>(argv_stream)),
                               std::istream_iterator<std::string>());
    return Ok(result);
  }

  Result<std::pair<int, int>, error::Err>
  Config_File::generate_socketpair() noexcept
  {
    int __fds[2] = { 0 };
    /* creating a Unix domain socket, and socket will use a communication semantic with
     * packets and fixed length datagrams.*/
    if (-1 == socketpair(AF_UNIX, SOCK_SEQPACKET, 0, __fds))
      return ERR(error::Code::Socket);

    return Ok(std::make_pair(__fds[0], __fds[1]));
  }

}