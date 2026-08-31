#include "Config.h"
#include "Downloader.h"
#include <string_view>
#include <unordered_map>

class ImageCreator { // Class reads a settings file that was created and builds
                     // an image based on that.
private:
  Downloader imageDownloader_;
  std::vector<minidocker::Instruction> instructions_;
  std::unordered_map<minidocker::BaseImage, std::string> baseLinkMap_ = {
      {minidocker::BaseImage::Alpine,
       "https://dl-cdn.alpinelinux.org/alpine/latest-stable/releases/x84_64/"
       "alpine-minirootfs-3.24.0-x86_64.tar.gz"}};

  const minidocker::FilePath tempFile_ = "base.tar.gz";

private:
  void DownloadImage(minidocker::From instruction);

  void CopyFile(minidocker::Copy instruction);
  void ChangeWorkDir(minidocker::Cwd instruction);
  void AddFolder(minidocker::Add instructions);

public:
  ImageCreator();

  void CreateImage();
};
