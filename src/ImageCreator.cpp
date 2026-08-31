#include "ImageCreator.h"
#include "Config.h"
#include <filesystem>
#include <string_view>
#include <variant>

ImageCreator::ImageCreator() {}

void ImageCreator::CreateImage() {

  for (auto &instruction : instructions_) {

    if (std::holds_alternative<minidocker::From>(instruction))
      continue;
    else if (std::holds_alternative<minidocker::Copy>(instruction))
      CopyFile(std::get<minidocker::Copy>(instruction));
    else if (std::holds_alternative<minidocker::Add>(instruction))
      AddFolder(std::get<minidocker::Add>(instruction));
    else if (std::holds_alternative<minidocker::Cwd>(instruction))
      ChangeWorkDir(std::get<minidocker::Cwd>(instruction));
  }
}

void ImageCreator::CopyFile(minidocker::Copy instruction) {

  if (!std::filesystem::is_directory(instruction.destination.parent_path())) {
    std::filesystem::create_directories(instruction.destination.parent_path());
  }

  if (!std::filesystem::exists(instruction.source)) {
    return;
  }

  std::filesystem::copy(instruction.source, instruction.destination,
                        std::filesystem::copy_options::overwrite_existing);
}
void ImageCreator::AddFolder(minidocker::Add instruction) {

  if (!std::filesystem::create_directories(instruction.folderPath)) {
    return;
  }
}

void ImageCreator::ChangeWorkDir(minidocker::Cwd instruction) {

  if (!std::filesystem::is_directory(instruction.WorkingDirectory)) {
    return;
  }
  std::filesystem::current_path(instruction.WorkingDirectory);
}

void ImageCreator::DownloadImage(minidocker::From instruction) {

  std::string baseLink = baseLinkMap_[instruction.baseImage];
  imageDownloader_.DownloadImage(baseLink, tempFile_);
  imageDownloader_.DeCompressArchive(tempFile_, "/var/lib/minidocker");
}
