#include <array>
#include <filesystem>
#include <string>
class App {
private:
  const std::filesystem::path defaultPath_ = "/var/lib/minidocker";
  std::array<std::string, 4> directories = {"bases", "images", "containers",
                                            "settings"};

private:
  void CreateAppFolders();

public:
  void Init(); // Create app files in /var/lib, and create log files.
  void Run();
};
