#include "app/App.h"
#include "Config.h"
#include "container/Container.h"
#include "image/ImageCreator.h"
#include "image/SettingsManager.h"
#include <exception>
#include <filesystem>
#include <iostream>

void App::Init() {

  try {
    for (auto &dir : directories) {
      std::filesystem::create_directories(defaultPath_ / dir);
    }
  } catch (const std::exception &e) {

    std::cerr << "App Files Failed To Initialize: " << e.what() << '\n';
  }
}

void App::Run() {
  SettingsManager settingsManager;

  ImageCreator imageCreator(settingsManager.GetInstructions());
  // imageCreator.CreateImage();

  minidocker::CgroupConfig Cgroupconfig{100000, 1, "512M"};
  minidocker::ContainerConfig config{
      Cgroupconfig,
      "test",
      false,
  };
  Container container(config);
  container.Init();
  std::vector<std::string> a = {"/bin/printenv", "VAR"};
  container.Run(a);
}
