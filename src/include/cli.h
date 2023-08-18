/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef __BONDING_CLI_H__
#define __BONDING_CLI_H__

#include "error.h"
#include "result.hpp"
#include "spdlog/spdlog.h"
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
    Parser(int argc, char ** argv)
      : m_argc(argc)
      , m_argv(argv)
      , m_required(0)
    {
    }

    struct cmd
    {
      std::string shorthand, value, descr;
      bool is_required, is_boolean;
    };

    Result<bool, error::Err> parse() noexcept;
    Result<Void, error::Err> help() const noexcept;

    Result<bool, error::Err>
    add(std::string const & name,
        std::string const & descr,
        std::string const & shorthand,
        bool is_required,
        bool is_boolean = false) noexcept;

    template <typename T>
    Result<T, error::Err>
    get(std::string const & name) const noexcept;

    Result<bool, error::Err>
    parsed(std::string const & name) const noexcept;

    template <typename T>
    Result<T, error::Err>
    parse(std::string const & value) const noexcept;

   private:
    int m_argc;
    char ** m_argv;
    int m_required;
    std::unordered_map<std::string, cmd> m_cmds;
    std::unordered_map<std::string, int> m_shorthands;
    std::vector<std::string> m_names;
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
    static Result<Void, error::Err> make(const int argc, char * argv[]) noexcept;
    static Result<Parser, error::Err> init_parser(const int argc, char * argv[]) noexcept;
  };

  Result<Void, error::Err> function(const Command_Line_Args args) noexcept;
  Result<Void, error::Err> init(const Command_Line_Args args) noexcept;
  Result<Void, error::Err> run(const Command_Line_Args args) noexcept;
};

#endif /* __BONDING_CLI_H__ */
