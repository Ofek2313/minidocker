#include "app/App.h"
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
  imageCreator.CreateImage();

  Container container;
  container.Init();
  std::vector<std::string> a = {"ps", "aux"};
  container.Run(a);
}
