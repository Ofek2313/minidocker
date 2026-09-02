#include "Config.h"
#include "Downloader.h"
#include "uuid/uuid.h"
#include <string_view>
#include <unordered_map>

class ImageCreator { // Class reads a settings file that was created and builds
                     // an image based on that.
private:
  Downloader imageDownloader_;
  std::vector<minidocker::Instruction> instructions_;
  std::unordered_map<minidocker::BaseImage, std::string> baseLinkMap_ = {
      {minidocker::BaseImage::Alpine,
       "https://dl-cdn.alpinelinux.org/alpine/latest-stable/releases/x86_64/"
       "alpine-minirootfs-3.24.0-x86_64.tar.gz"}};

  const minidocker::FilePath tempFile_ = "/tmp/base.tar.gz";

  minidocker::FilePath imageFolderPath_ = "/var/lib/minidocker/images";
  uuid_t imageId;

private:
  void ApplyInstruction(minidocker::From instruction);
  void ApplyInstruction(minidocker::Copy instruction);
  void ApplyInstruction(minidocker::Cwd instruction);
  void ApplyInstruction(minidocker::Add instruction);
  void CreateImageFolder();

public:
  ImageCreator();
  ImageCreator(std::vector<minidocker::Instruction> instructions);
  void CreateImage();
};
