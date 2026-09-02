#include "ImageCreator.h"
#include "Config.h"

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include <sys/mount.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <variant>

ImageCreator::ImageCreator() {}

ImageCreator::ImageCreator(std::vector<minidocker::Instruction> instructions)
    : instructions_{instructions} {}

void ImageCreator::CreateImageFolder() {

  uuid_generate(imageId);
  char uuidText[37];
  uuid_unparse(imageId, uuidText);
  imageFolderPath_ = imageFolderPath_ / uuidText;
  std::filesystem::create_directory(imageFolderPath_);
}

void ImageCreator::CreateImage() {
  CreateImageFolder();
  minidocker::From from{minidocker::BaseImage::Alpine};
  ApplyInstruction(from);
  for (auto &instruction : instructions_) {

    std::visit(
        [this](const auto &instruction) { ApplyInstruction(instruction); },
        instruction);
  }
}

void ImageCreator::ApplyInstruction(minidocker::Copy instruction) {

  if (!std::filesystem::is_directory(instruction.destination.parent_path())) {
    std::filesystem::create_directories(instruction.destination.parent_path());
  }

  if (!std::filesystem::exists(instruction.source)) {
    return;
  }

  std::filesystem::copy(instruction.source, instruction.destination,
                        std::filesystem::copy_options::overwrite_existing);
}
void ImageCreator::ApplyInstruction(minidocker::Add instruction) {

  if (!std::filesystem::create_directories(instruction.folderPath)) {
    return;
  }
}

void ImageCreator::ApplyInstruction(minidocker::Cwd instruction) {

  if (!std::filesystem::is_directory(instruction.WorkingDirectory)) {
    return;
  }
  std::filesystem::current_path(instruction.WorkingDirectory);
}

void ImageCreator::ApplyInstruction(minidocker::From instruction) {

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
