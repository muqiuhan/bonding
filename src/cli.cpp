/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/cli.h"
#include "include/bonding.hpp"
#include "include/configfile.h"
#include "include/container.h"
#include "logging.h"
#include "include/unix.h"
#include <cstdlib>
#include <error.h>
#include <iterator>
#include <vector>

namespace bonding::cli
{
  std::expected<void, error::Err>
    Command_Line_Args::make(const int argc, char * argv[]) noexcept
  {
    return function(init_parser(argc, argv).value());
  }

  std::expected<Parser, error::Err>
    Command_Line_Args::init_parser(const int argc, char * argv[]) noexcept
  {
    Parser parser(argc, argv);

    parser
      .add(
        "init",
        "Initialize the current directory as the container directory",
        "init",
        false,
        true)
      .value();

    parser
      .add(
        "run",
        "Run with the current directory as the container directory",
        "run",
        false,
        true)
      .value();

    parser.add("help", "show this message", "help", false, true).value();

    parser.add("version", "show the version of bonding", "version", false, true).value();

    if (argc == 1)
      {
        parser.help().value();
        exit(EXIT_SUCCESS);
      }

    if (!parser.parse().has_value())
      {
        parser.help().value();
        return std::unexpected(
          ERR_MSG(error::Code::Cli, "Cannot parse the command line argument"));
      }

    return parser;
  }

  std::expected<bool, error::Err> Parser::parse() noexcept
  {
    assert(m_argc > 0);
    if (m_argc - 1 < m_required)
      return std::unexpected(ERR(error::Code::Cli));

    int                             num_required = 0;
    std::unordered_set<std::string> parsed_shorthands;
    parsed_shorthands.reserve(m_argc);

    for (int i = 1; i != m_argc; ++i)
      {
        std::string parsed(m_argv[i]);
        if (parsed == "-h" || parsed == "--help")
          return std::unexpected(ERR(error::Code::Cli));

        int id = 0;
        if (const auto it = m_shorthands.find(parsed); it == m_shorthands.end())
          return std::unexpected(
            ERR_MSG(error::Code::Cli, "shorthand " + parsed + " not found"));

        else
          {
            if (const auto it = parsed_shorthands.find(parsed);
                it != parsed_shorthands.end())
              return std::unexpected(
                ERR_MSG(error::Code::Cli, "shorthand '" + parsed + "' already parsed"));

            parsed_shorthands.emplace(parsed);
            id = (*it).second;
          }

        assert(static_cast<size_t>(id) < m_names.size());
        auto const & name = m_names[id];
        auto &       cmd = m_cmds[name];
        if (cmd.is_required)
          num_required += 1;
        if (cmd.is_boolean)
          parsed = "true";
        else
          {
            ++i;
            if (i == m_argc)
              return std::unexpected(ERR(error::Code::Cli));

            parsed = m_argv[i];
          }
        cmd.value = parsed;
      }

    if (num_required != m_required)
      return std::unexpected(ERR(error::Code::Cli));

    return true;
  }

  std::expected<void, error::Err> Parser::help() const noexcept
  {
    std::cerr << "Usage: " << m_argv[0] << " [help]";
    const auto print = [this](bool with_description) {
      for (size_t i = 0; i != m_names.size(); ++i)
        {
          auto const & cmd = m_cmds.at(m_names[i]);

          std::cout << " [" << cmd.shorthand;

          if (!cmd.is_boolean)
            std::cerr << " " << m_names[i];

          std::cout << "]";

          if (with_description)
            std::cout << "\n\t" + std::string((cmd.is_required ? "REQUIRED: " : ""))
                           + cmd.descr + "\n\n";
        }
    };

    print(false);
    std::cout << "\n\n";
    print(true);
    return {};
  }

  std::expected<bool, error::Err> Parser::add(
    std::string const & name,
    std::string const & descr,
    std::string const & shorthand,
    bool                is_required,
    bool                is_boolean) noexcept
  {
    bool ret =
      m_cmds
        .emplace(
          name, cmd{shorthand, is_boolean ? "false" : "", descr, is_required, is_boolean})
        .second;
    if (ret)
      {
        if (is_required)
          m_required += 1;
        m_names.push_back(name);
        m_shorthands.emplace(shorthand, m_names.size() - 1);
      }
    return ret;
  }

  template <typename T>
  [[nodiscard]] std::expected<T, error::Err>
    Parser::get(std::string const & name) const noexcept
  {
    auto it = m_cmds.find(name);
    if (it == m_cmds.end())
      return std::unexpected(
        ERR_MSG(error::Code::Cli, "error: '" + name + "' not found"));

    auto const & value = (*it).second.value;
    return parse<T>(value).value();
  }

  [[maybe_unused]] [[nodiscard]] std::expected<bool, error::Err>
    Parser::parsed(std::string const & name) const noexcept
  {
    auto it = m_cmds.find(name);
    if (it == m_cmds.end())
      return false;
    auto const & cmd = (*it).second;
    if (cmd.is_boolean)
      {
        if (cmd.value == "true")
          return true;
        if (cmd.value == "false")
          return false;
        assert(false); // should never happen
      }

    return cmd.value != "";
  }

  template <typename T>
  [[nodiscard]] std::expected<T, error::Err>
    Parser::parse(std::string const & value) const noexcept
  {
    if constexpr (std::is_same<T, std::string>::value)
      return value;
    else if constexpr (
      std::is_same<T, char>::value || std::is_same<T, signed char>::value
      || std::is_same<T, unsigned char>::value)
      return value.front();
    else if constexpr (
      std::is_same<T, unsigned int>::value || std::is_same<T, int>::value
      || std::is_same<T, unsigned short int>::value || std::is_same<T, short int>::value)
      return std::strtol(value.c_str(), nullptr, 10);
    else if constexpr (
      std::is_same<T, unsigned long int>::value || std::is_same<T, long int>::value
      || std::is_same<T, unsigned long long int>::value
      || std::is_same<T, long long int>::value)
      return std::strtoll(value.c_str(), nullptr, 10);
    else if constexpr (
      std::is_same<T, float>::value || std::is_same<T, double>::value
      || std::is_same<T, long double>::value)
      return std::strtod(value.c_str(), nullptr);
    else if constexpr (std::is_same<T, bool>::value)
      {
        std::istringstream stream(value);
        bool               ret;
        if (value == "true" || value == "false")
          stream >> std::boolalpha >> ret;
        else
          stream >> std::noboolalpha >> ret;
        return ret;
      }
    assert(false); // should never happen
    return std::unexpected(ERR_MSG(error::Code::Cli, "unsupported type"));
  }

  [[nodiscard]] std::expected<void, error::Err> function(const Parser parser) noexcept
  {
    if (parser.get<bool>("init").value())
      return init(parser);
    else if (parser.get<bool>("run").value())
      return run(parser);
    else if (parser.get<bool>("version").value())
      return version(parser);
    else if (parser.get<bool>("help").value())
      return parser.help();
    else
      return std::unexpected(ERR(error::Code::Cli));

    return {};
  }

  [[nodiscard]] std::expected<void, error::Err> run(const Parser & args) noexcept
  {
    return container::Container::start(
      configfile::Config_File::read("./bonding.json").value());

    return {};
  }

  [[nodiscard]] std::expected<void, error::Err> init(const Parser & args) noexcept
  {
    std::string hostname;
    std::string command;

    std::cout << "Hostname: ";
    std::getline(std::cin, hostname);

    std::cout << "Command: ";
    std::getline(std::cin, command);

    unix::Filesystem::Mkdir("Bonding." + hostname).value();
    unix::Filesystem::Mkdir("Bonding." + hostname + "/mount_dir").value();

    unix::Filesystem::Write(
      "Bonding." + hostname + "/bonding.json",
      configfile::Config_File::generate_default(hostname, command).value())
      .value();

    LOG_INFO << "Creating a Container...✓";

    return {};
  }

  [[nodiscard]] std::expected<void, error::Err> version(const Parser & args) noexcept
  {
    std::cout << "Welcome to Bonding v" + BONDING_VERSION + " [" + BONDING_COPYRIGHT + "]"
              << std::endl;

    return {};
  }

} // namespace bonding::cli
