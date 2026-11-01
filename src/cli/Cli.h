

#include <optional>
#include <string_view>
#include <vector>
class Cli {

public:
  explicit Cli(int argc, char *argv[]);
  void Run();

private:
  std::vector<std::string_view> args_;

private:
  void Validate();
  bool IsValidCommand(std::string_view command) const;
  bool IsParamValid(std::string_view parameter, std::string_view command) const;
  bool ValidateParameters();
  std::vector<std::string> Split(const std::string &string, char splitChar);
  std::optional<std::string_view> GetParameter(std::string_view flag) const;
};
