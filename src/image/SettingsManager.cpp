#include "image/SettingsManager.h"
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>

SettingsManager::SettingsManager() {}

void SettingsManager::LoadSettings() {

  std::ifstream jsonFile(settingsPath_);
  if (!jsonFile)
    throw;

  minidockerImageConfig_ = Json::parse(jsonFile);
}

std::vector<instructions::Instruction> SettingsManager::GetInstructions() {

  std::vector<instructions::Instruction> instructions;

  for (auto &element : minidockerImageConfig_) {

    commandFormat format;
    format.command = element["command"];
    std::string arg;
    std::string args = element["args"];
    std::stringstream ss(args);
    while (ss >> arg) {
      format.args.push_back(arg);
    }

    if (format.command == "COPY") {
      instructions::Copy copy{format.args[0], format.args[1]};
      instructions.push_back(copy);
    } else if (format.command == "ADD") {
      instructions::Add add{format.args[0]};
      instructions.push_back(add);
    } else if (format.command == "CWD") {
      instructions::Cwd cwd{format.args[0]};
      instructions.push_back(cwd);
    } else if (format.command == "FROM") {
      instructions::From from{baseMap_[format.args[0]]};
      instructions.push_back(from);
    }
  }

  return instructions;
}
