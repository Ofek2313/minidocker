#include "RootFileSystem.h"
#include "Downloader.h"
#include <filesystem>
#include <string_view>
#include <unistd.h>
#include <zlib.h>

bool RootFileSystem::setRoot(int pid) {

  using filepath = std::filesystem::path;
  filepath symPath = "/proc/" + std::to_string(getpid()) + "/exe";
  filepath procPath = std::filesystem::read_symlink(symPath);
  int status = chroot(ROOTPATH.data());
  return (status == 0) ? true : false;
}

void RootFileSystem::DownloadAlpineEnvironment() {

  Downloader downloader;
  constexpr std::string_view ALPINEURL =
      "https://dl-cdn.alpinelinux.org/alpine/latest-stable/releases/x86_64/"
      "alpine-minirootfs-3.24.0-x86_64.tar.gz";
  constexpr std::string_view PATH = "/tmp/alpine.tar.gz";
  downloader.DownloadImage(ALPINEURL, PATH);
  downloader.DeCompressArchive(PATH);
}
