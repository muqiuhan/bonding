/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/cli.h"
#include "include/bonding.hpp"
#include "include/configfile.h"
#include "include/container.h"
#include "include/hostname.h"
#include "include/log.hpp"
#include <cstdlib>
#include <error.h>
#include <iterator>
#include <vector>

namespace bonding::cli
{
  Result<Void, error::Err>
  Command_Line_Args::make(const int argc, char * argv[]) noexcept
  {
    return function(init_parser(argc, argv).unwrap());
  }

  Result<Parser, error::Err>
  Command_Line_Args::init_parser(const int argc, char * argv[]) noexcept
  {
    Parser parser(argc, argv);

    parser.add("init",
               "Initialize the current directory as the container directory",
               "init",
               false,
               true);

    parser.add("run",
               "Run with the current directory as the container directory",
               "run",
               false,
               true);

    parser.add("help", "show this message", "help", false, true);

    parser.add("version", "show the version of bonding", "version", false, true);

    if (argc == 1)
      {
        parser.help().unwrap();
        exit(EXIT_SUCCESS);
      }

    if (parser.parse().is_err())
      {
        parser.help().unwrap();
        return ERR_MSG(error::Code::Cli, "Cannot parse the command line argument");
      }

    return Ok(parser);
  }

  Result<bool, error::Err>
  Parser::parse() noexcept
  {
    assert(m_argc > 0);
    if (m_argc - 1 < m_required)
      return ERR(error::Code::Cli);

    int num_required = 0;
    std::unordered_set<std::string> parsed_shorthands;
    parsed_shorthands.reserve(m_argc);

    for (int i = 1; i != m_argc; ++i)
      {
        std::string parsed(m_argv[i]);
        if (parsed == "-h" || parsed == "--help")
          return ERR(error::Code::Cli);

        int id = 0;
        if (const auto it = m_shorthands.find(parsed); it == m_shorthands.end())
          return ERR_MSG(error::Code::Cli, "shorthand " + parsed + " not found");

        else
          {
            if (const auto it = parsed_shorthands.find(parsed);
                it != parsed_shorthands.end())
              return ERR_MSG(error::Code::Cli,
                             "shorthand '" + parsed + "' already parsed");

            parsed_shorthands.emplace(parsed);
            id = (*it).second;
          }

        assert(static_cast<size_t>(id) < m_names.size());
        auto const & name = m_names[id];
        auto & cmd = m_cmds[name];
        if (cmd.is_required)
          num_required += 1;
        if (cmd.is_boolean)
          parsed = "true";
        else
          {
            ++i;
            if (i == m_argc)
              return ERR(error::Code::Cli);
            parsed = m_argv[i];
          }
        cmd.value = parsed;
      }

    if (num_required != m_required)
      return ERR(error::Code::Cli);

    return Ok(true);
  }

  Result<Void, error::Err>
  Parser::help() const noexcept
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

    return Ok(Void());
  }

  Result<bool, error::Err>
  Parser::add(std::string const & name,
              std::string const & descr,
              std::string const & shorthand,
              bool is_required,
              bool is_boolean) noexcept
  {
    bool ret =
      m_cmds
        .emplace(
          name,
          cmd{ shorthand, is_boolean ? "false" : "", descr, is_required, is_boolean })
        .second;
    if (ret)
      {
        if (is_required)
          m_required += 1;
        m_names.push_back(name);
        m_shorthands.emplace(shorthand, m_names.size() - 1);
      }
    return Ok(ret);
  }

  template <typename T>
  Result<T, error::Err>
  Parser::get(std::string const & name) const noexcept
  {
    auto it = m_cmds.find(name);
    if (it == m_cmds.end())
      return ERR_MSG(error::Code::Cli, "error: '" + name + "' not found");

    auto const & value = (*it).second.value;
    return Ok(parse<T>(value).unwrap());
  }

  [[maybe_unused]] Result<bool, error::Err>
  Parser::parsed(std::string const & name) const noexcept
  {
    auto it = m_cmds.find(name);
    if (it == m_cmds.end())
      return Ok(false);
    auto const & cmd = (*it).second;
    if (cmd.is_boolean)
      {
        if (cmd.value == "true")
          return Ok(true);
        if (cmd.value == "false")
          return Ok(false);
        assert(false); // should never happen
      }

    return Ok(cmd.value != "");
  }

  template <typename T>
  Result<T, error::Err>
  Parser::parse(std::string const & value) const noexcept
  {
    if constexpr (std::is_same<T, std::string>::value)
      return value;
    else if constexpr (std::is_same<T, char>::value || std::is_same<T, signed char>::value
                       || std::is_same<T, unsigned char>::value)
      return value.front();
    else if constexpr (std::is_same<T, unsigned int>::value || std::is_same<T, int>::value
                       || std::is_same<T, unsigned short int>::value
                       || std::is_same<T, short int>::value)
      return std::strtol(value.c_str(), nullptr, 10);
    else if constexpr (std::is_same<T, unsigned long int>::value
                       || std::is_same<T, long int>::value
                       || std::is_same<T, unsigned long long int>::value
                       || std::is_same<T, long long int>::value)
      return std::strtoll(value.c_str(), nullptr, 10);
    else if constexpr (std::is_same<T, float>::value || std::is_same<T, double>::value
                       || std::is_same<T, long double>::value)
      return std::strtod(value.c_str(), nullptr);
    else if constexpr (std::is_same<T, bool>::value)
      {
        std::istringstream stream(value);
        bool ret;
        if (value == "true" || value == "false")
          stream >> std::boolalpha >> ret;
        else
          stream >> std::noboolalpha >> ret;
        return Ok(ret);
      }
    assert(false); // should never happen
    return ERR_MSG(error::Code::Cli, "unsupported type");
  }

  Result<Void, error::Err>
  function(const Parser parser) noexcept
  {
    if (parser.get<bool>("init").unwrap())
      return init(parser);
    else if (parser.get<bool>("run").unwrap())
      return run(parser);
    else if (parser.get<bool>("version").unwrap())
      return version(parser);
    else if (parser.get<bool>("help").unwrap())
      return parser.help();
    else
      return ERR(error::Code::Cli);

    return Ok(Void());
  }

  Result<Void, error::Err>
  run(const Parser & args) noexcept
  {
    return Ok(container::Container::start(configfile::Config_File::read("./bonding.json"))
                .unwrap());
  }

  Result<Void, error::Err>
  init(const Parser & args) noexcept
  {
    return ERR_MSG(error::Code::Capabilities, "Not implemented");
  }

  Result<Void, error::Err>
  version(const Parser & args) noexcept
  {
    std::cout << "Welcome to Bonding v" + BONDING_VERSION + " [" + BONDING_COPYRIGHT + "]"
              << std::endl;
    return Ok(Void());
  }

}
