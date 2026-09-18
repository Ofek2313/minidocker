#include "image/ImageCreator.h"
#include "Config.h"
#include "image/Instructions.h"

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <sys/mount.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <variant>

ImageCreator::ImageCreator() {}

ImageCreator::ImageCreator(std::vector<instructions::Instruction> instructions)
    : instructions_{std::move(instructions)} {}

void ImageCreator::CreateImageFolder(std::string_view imageName = {}) {

  if (imageName.empty()) {
    uuid_t imageId;
    uuid_generate(imageId);
    char uuidText[37];
    uuid_unparse(imageId, uuidText);
    imageFolderPath_ = baseFolderPath_ / uuidText;
    std::cout << "No Image Name, Image created as:  " << uuidText << '\n';
  } else
    imageFolderPath_ = baseFolderPath_ / imageName;

  if (!std::filesystem::create_directory(imageFolderPath_)) {
    std::cerr << "Image with that name already exists" << '\n';
  } // create directory throws, so I let it bubble up and catch it.
}

void ImageCreator::CreateEnvFile() {
  if (!envVariables_.is_null()) {

    std::ofstream file(imageFolderPath_ / "env.json");

    if (file) {
      file << envVariables_;
      file.close();
    }
  }
}

void ImageCreator::CreateImage() {
  CreateImageFolder("test");
  instructions::From from{instructions::BaseImage::Alpine};
  instructions::Env env{"VAR", "5"};
  ApplyInstruction(from);
  ApplyInstruction(env);
  for (auto &instruction : instructions_) {

    std::visit(
        [this](const auto &instruction) { ApplyInstruction(instruction); },
        instruction);
  }
  CreateEnvFile();
}

void ImageCreator::ApplyInstruction(instructions::Copy instruction) {

  if (!std::filesystem::is_directory(instruction.destination.parent_path())) {
    std::filesystem::create_directories(instruction.destination.parent_path());
  }

  if (!std::filesystem::exists(instruction.source)) {
    return;
  }

  std::filesystem::copy(instruction.source, instruction.destination,
                        std::filesystem::copy_options::overwrite_existing);
}
void ImageCreator::ApplyInstruction(instructions::Add instruction) {

  if (!std::filesystem::create_directories(instruction.folderPath)) {
    return;
  }
}

void ImageCreator::ApplyInstruction(instructions::Cwd instruction) {

  if (!std::filesystem::is_directory(instruction.WorkingDirectory)) {
    return;
  }
  std::filesystem::current_path(instruction.WorkingDirectory);
}

void ImageCreator::ApplyInstruction(instructions::Env instruction) {
  // Create a json file to save env variables env.json
  envVariables_[instruction.name] = instruction.value;
}

void ImageCreator::ApplyInstruction(instructions::From instruction) {

  std::string baseLink = baseLinkMap_[instruction.baseImage];
  minidocker::FilePath basePath = "/var/lib/minidocker/bases/alpine";
  std::filesystem::create_directory(basePath);
  imageDownloader_.DownloadImage(baseLink, tempFile_);
  imageDownloader_.DeCompressArchive(
      tempFile_,
      basePath); // Download from web if does not exists, must also save a
                 // cahce of already downlaoded and preform a check.

  mount(basePath.c_str(), imageFolderPath_.c_str(), NULL, MS_BIND, NULL);
}
