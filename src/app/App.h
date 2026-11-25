#pragma once
#include "Config.h"
#include "shared/Options.h"
#include <array>
#include <filesystem>
#include <string>
class App {
private:
  const std::filesystem::path defaultPath_ = "/var/lib/minidocker";
  std::array<std::string, 4> directories = {"bases", "images", "containers",
                                            "settings"};

  minidocker::ContainerConfig config_;

private:
  void CreateAppFolders();

public:
  void Init(); // Create app files in /var/lib, and create log files.
  void Run(minidocker::RunOptions &runOptions);
  void Build(minidocker::BuildOptions &buildOptions);

  App(minidocker::RunOptions &runOptions);
  App(minidocker::BuildOptions &buildOptions);
  App() = default;
};
