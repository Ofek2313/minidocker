#include "cli/Cli.h"
#include <algorithm>
#include <array>
#include <complex>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>

namespace {

using parametersList = std::vector<std::string_view>;

constexpr std::array<std::string_view, 4> validCommands = {"build", "run",
                                                           "status", "kill"};

const std::unordered_map<std::string_view, parametersList> vaildParams = {
    {"run", {"hello", "name"}}};
} // namespace

bool Cli::IsValidCommand(std::string_view command) const {

  auto itr = std::find(validCommands.begin(), validCommands.end(), args_.at(0));
  return itr != validCommands.end();
}

bool Cli::IsParamValid(std::string_view parameter,
                       std::string_view command) const {
  parametersList commandParamList = vaildParams.at(command);
  auto it =
      std::find(commandParamList.begin(), commandParamList.end(), parameter);
  return it != commandParamList.end();
}

Cli::Cli(int argc, char *argv[]) : args_(argv + 1, argc + argv) {}

bool Cli::ValidateParameters() {

  for (auto itr = std::next(args_.begin() + 1); itr != args_.end(); ++itr) {

    if ((*itr).starts_with("--")) {
      std::string param = "";
      for (char character : (*itr)) {
        if (character == '=') {
          bool valid = IsParamValid(param, args_.at(0));
          if (!valid)
            return false;
        }
        param += character;
      }

      size_t pos = (*itr).find('=');
      return pos != std::string::npos;
    }
  }
  return true;
}

void Cli::Validate() {
  // Check if first command is valid then check follow up parameters
  //
  if (args_.size() < 1)
    return;

  //
}

std::vector<std::string> Cli::Split(const std::string &string, char splitChar) {

  std::stringstream ss(string);
  std::vector<std::string> splitedString;
  std::string arg;
  while (std::getline(ss, arg, splitChar)) {

    splitedString.push_back(arg);
    std::cout << arg << '\n';
  }
  return splitedString;
}

std::optional<std::string_view> Cli::GetParameter(std::string_view flag) const {
  // check if flag exist
  auto itr = std::find(args_.begin(), args_.end(), flag);

  if (itr == args_.end()) {

    for (std::string_view arg : args_) {

      if (arg.size() > flag.size() && arg.starts_with(flag) &&
          arg[flag.size()] == '=') {
        std::cout << arg.substr(flag.size() + 1);
        return arg.substr(flag.size() + 1);
      }
      // if we find the paramteer leading with -- then split
      // the string and with a "=" check if it does
      // exist(vector length == 2), and return it;
    }

    // for (auto it = parameterizedString.begin(); it <
    // parameterizedString.end();
    //      ++it) {
    // }
    //  itr =
    //  std::find(parameterizedString.begin(),parameterizedString.end(),flag);
    return std::nullopt;
  }

  if (std::next(itr) == args_.end())
    return std::nullopt;

  return *(std::next(itr));

  // check if follow up parameters exist
  //
  // find it in the args make sure in bounds and return
}

void Cli::Run() {}
