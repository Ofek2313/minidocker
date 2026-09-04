#pragma once
#include "image/Instructions.h"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>
#include <vector>

using Json = nlohmann::json;
namespace instructions = minidocker::image::instuctions;
class SettingsManager {

private:
  struct commandFormat {
    std::string command;
    std::vector<std::string> args;
  };
  Json minidockerImageConfig_;
  const minidocker::FilePath settingsPath_ = "/var/lib/minidocker/settings";
  std::unordered_map<std::string, instructions::BaseImage> baseMap_ = {

      {"alpine", instructions::BaseImage::Alpine},
      {"ubuntu", instructions::BaseImage::Ubuntu}};

private:
  instructions::Instruction constructInstruction(commandFormat &format);
  void LoadSettings();

public:
  std::vector<instructions::Instruction> GetInstructions();
  void WriteSettings(); // Load a struct maybe the write or append to a json
                        // file. pair it with a cli.
  SettingsManager();
};
