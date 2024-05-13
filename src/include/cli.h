/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_CLI_H
#define BONDING_CLI_H

#include "error.h"
#include "log.hpp"
#include "result.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace bonding::cli
{
  struct Parser
  {
    Parser(int argc, char ** argv) : m_argc(argc), m_argv(argv), m_required(0) {}

    Parser() = default;

    struct cmd
    {
      std::string shorthand, value, descr;
      bool        is_required, is_boolean;
    };

    Result<bool, error::Err> parse() noexcept;
    Result<Void, error::Err> help() const noexcept;

    Result<bool, error::Err> add(
      std::string const & name,
      std::string const & descr,
      std::string const & shorthand,
      bool                is_required,
      bool                is_boolean = false) noexcept;

    template <typename T>
    Result<T, error::Err> get(std::string const & name) const noexcept;

    [[maybe_unused]] Result<bool, error::Err>
      parsed(std::string const & name) const noexcept;

    template <typename T>
    Result<T, error::Err> parse(std::string const & value) const noexcept;

  private:
    int                                  m_argc;
    char **                              m_argv;
    int                                  m_required;
    std::unordered_map<std::string, cmd> m_cmds;
    std::unordered_map<std::string, int> m_shorthands;
    std::vector<std::string>             m_names;
  };

  enum class Mode
  {
    Init,
    Run,
    Help
  };

  class Command_Line_Args
  {
  public:
    static Result<Void, error::Err>   make(int argc, char * argv[]) noexcept;
    static Result<Parser, error::Err> init_parser(int argc, char * argv[]) noexcept;
  };

  Result<Void, error::Err> function(const Parser args) noexcept;
  Result<Void, error::Err> run(const Parser & args) noexcept;
  Result<Void, error::Err> init(const Parser & args) noexcept;
  Result<Void, error::Err> version(const Parser & args) noexcept;
}; // namespace bonding::cli

#endif /* BONDING_CLI_H */
