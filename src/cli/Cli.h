#pragma once
#include "shared/Options.h"
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
namespace minidocker {} // namespace minidocker
class Cli {
  using Options = std::unordered_map<std::string, std::string>;

public:
  explicit Cli(int argc, char *argv[]);
  void Interpret();

  std::optional<std::string_view> GetParameter(std::string_view flag) const;

private:
  std::vector<std::string_view> args_;

private:
  void Validate();
  bool IsValidCommand(std::string_view command) const;
  bool IsParamValid(std::string_view parameter, std::string_view command) const;
  bool ValidateParameters();
  std::vector<std::string> Split(const std::string &string, char splitChar);
  Options ParaseArgs();

  minidocker::RunOptions BuildFromArgs(Options ParasedArgs);
};
