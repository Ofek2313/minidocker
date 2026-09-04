#pragma once
#include "Instructions.h"
#include "tools/Downloader.h"
#include "uuid/uuid.h"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string_view>
#include <unordered_map>

namespace instructions = minidocker::image::instuctions;

using Json = nlohmann::json;
class ImageCreator { // Class reads a settings file that was created and builds
                     // an image based on that.

private:
  Downloader imageDownloader_;
  std::vector<instructions::Instruction> instructions_;
  std::unordered_map<instructions::BaseImage, std::string> baseLinkMap_ = {
      {instructions::BaseImage::Alpine,
       "https://dl-cdn.alpinelinux.org/alpine/latest-stable/releases/x86_64/"
       "alpine-minirootfs-3.24.0-x86_64.tar.gz"}};

  const minidocker::FilePath tempFile_ = "/tmp/base.tar.gz";

  minidocker::FilePath imageFolderPath_ = "/var/lib/minidocker/images";
  uuid_t imageId;

  Json envVariables_;

private:
  void ApplyInstruction(instructions::From instruction);
  void ApplyInstruction(instructions::Copy instruction);
  void ApplyInstruction(instructions::Cwd instruction);
  void ApplyInstruction(instructions::Add instruction);
  void ApplyInstruction(instructions::Env instruction);

  void CreateImageFolder();
  void CreateEnvFile();

public:
  ImageCreator();
  ImageCreator(std::vector<instructions::Instruction> instructions);
  void CreateImage();
};
