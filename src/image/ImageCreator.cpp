#include "image/ImageCreator.h"
#include "Config.h"
#include "image/Instructions.h"
#include "shared/Paths.h"
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

namespace {
const minidocker::FilePath tempFile_ = "/tmp/base.tar.gz";
const minidocker::FilePath baseFolderPath_ = "/var/lib/minidocker/images";
} // namespace
ImageCreator::ImageCreator() {}

ImageCreator::ImageCreator(std::vector<instructions::Instruction> instructions)
    : instructions_{std::move(instructions)} {}

void ImageCreator::CreateImageFolder(std::string_view imageName = {}) {

  if (imageName.empty()) {
    uuid_t imageId;
    uuid_generate(imageId);
    char uuidText[37];
    uuid_unparse(imageId, uuidText);
    imageFolderPath_ = minidocker::FilePath(kImagePathDefault) / uuidText;
    std::cout << "No Image Name, Image created as:  " << uuidText << '\n';
  } else
    imageFolderPath_ = minidocker::FilePath(kImagePathDefault) / imageName;

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
  CreateImageFolder("image1");
  instructions::From from{instructions::BaseImage::Alpine};
  instructions::Add add{"/test"};
  instructions::Env env{"VAR", "5"};
  ApplyInstruction(from);
  ApplyInstruction(env);
  ApplyInstruction(add);
  for (auto &instruction : instructions_) {

    std::visit(
        [this](const auto &instruction) { ApplyInstruction(instruction); },
        instruction);
  }
  CreateEnvFile();
}

void ImageCreator::ApplyInstruction(const instructions::Copy &instruction) {

  if (!std::filesystem::is_directory(instruction.destination.parent_path())) {
    std::filesystem::create_directories(instruction.destination.parent_path());
  }

  if (!std::filesystem::exists(instruction.source)) {
    return;
  }

  std::filesystem::copy(instruction.source, instruction.destination,
                        std::filesystem::copy_options::overwrite_existing);
}
void ImageCreator::ApplyInstruction(const instructions::Add &instruction) {
  const minidocker::FilePath addFolderPath =
      imageFolderPath_ / "merged" / instruction.folderPath;
  if (!std::filesystem::create_directories(addFolderPath)) {
    return;
  }
}

void ImageCreator::ApplyInstruction(const instructions::Cwd &instruction) {

  if (!std::filesystem::is_directory(instruction.WorkingDirectory)) {
    return;
  }
  std::filesystem::current_path(instruction.WorkingDirectory);
}

void ImageCreator::ApplyInstruction(const instructions::Env &instruction) {
  // Create a json file to save env variables env.json
  envVariables_[instruction.name] = instruction.value;
}

void ImageCreator::ApplyInstruction(const instructions::From &instruction) {

  std::string baseLink = baseLinkMap_[instruction.baseImage];
  minidocker::FilePath basePath = "/var/lib/minidocker/bases/alpine";
  std::filesystem::create_directory(basePath);
  imageDownloader_.DownloadImage(baseLink, tempFile_);
  imageDownloader_.DeCompressArchive(
      tempFile_,
      basePath); // Download from web if does not exists, must also save a
                 // cahce of already downlaoded and preform a check.

  // mount(basePath.c_str(), imageFolderPath_.c_str(), NULL, MS_BIND, NULL);

  const std::string kWorkPath = std::string(imageFolderPath_) + "/work";
  const std::string kMergedPath = std::string(imageFolderPath_) + "/merged";
  const std::string kUpperPath = std::string(imageFolderPath_) + "/upper";
  std::cout << imageFolderPath_ << '\n';
  std::filesystem::create_directories(kWorkPath);
  std::filesystem::create_directories(kMergedPath);
  std::filesystem::create_directories(kUpperPath);
  std::string options = "lowerdir=" + basePath.string() +
                        ",upperdir=" + kUpperPath + ",workdir=" + kWorkPath;
  minidocker::checkErr(
      mount("overlay", kMergedPath.c_str(), "overlay", 0, options.c_str()),
      "Overlay filesystem failed");
}
ImageCreator::~ImageCreator() {

  const std::string kWorkPath = std::string(imageFolderPath_) + "/work";
  const std::string kMergedPath = std::string(imageFolderPath_) + "/merged";
  minidocker::checkErr(umount(kMergedPath.c_str()), "Failed to unmount image");
  std::filesystem::remove(kMergedPath);
  std::filesystem::remove_all(kWorkPath);
}
