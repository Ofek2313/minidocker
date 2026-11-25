#include "app/App.h"
#include "Config.h"
#include "container/Container.h"
#include "image/ImageCreator.h"
#include "image/SettingsManager.h"
#include "shared/Options.h"
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

App::App(minidocker::RunOptions &runOptions) {}

void App::Build(minidocker::BuildOptions &buildOptions) {
  SettingsManager settingsManager;

  ImageCreator imageCreator(settingsManager.GetInstructions());
  imageCreator.CreateImage();
}

void App::Run(minidocker::RunOptions &runOptions) {

  minidocker::CgroupConfig Cgroupconfig{runOptions.duration, runOptions.cores,
                                        runOptions.memory};
  config_ = {
      Cgroupconfig,
      runOptions.image,
      runOptions.detached,
  };
  Container container(config_);
  container.Init();
  std::cout << "running... " << std::endl;
  std::vector<std::string> a = {"/bin/echo", "hello"};
  container.Run(runOptions.commands);
}
