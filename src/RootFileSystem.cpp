#include "RootFileSystem.h"
#include "Downloader.h"
#include <cstddef>
#include <filesystem>
#include <string_view>
#include <sys/mount.h>
#include <unistd.h>
#include <zlib.h>

bool RootFileSystem::setRoot(int pid) {

  using filepath = std::filesystem::path;
  filepath symPath = "/proc/" + std::to_string(getpid()) + "/exe";
  filepath procPath = std::filesystem::read_symlink(symPath);
  int status = chroot(ROOTPATH.data());
  chdir("/");
  return (status == 0) ? true : false;
}

void RootFileSystem::DownloadAlpineEnvironment() {

  Downloader downloader;
  std::string ALPINEURL =
      "https://dl-cdn.alpinelinux.org/alpine/latest-stable/releases/x86_64/"
      "alpine-minirootfs-3.24.0-x86_64.tar.gz";
  std::string PATH = "/tmp/alpine.tar.gz";
  downloader.DownloadImage(ALPINEURL, PATH);
  downloader.DeCompressArchive(PATH, "/var/lib/minidocker");
}
bool RootFileSystem::IsRootFsInitialized() {

  if (!std::filesystem::is_directory(ROOTPATH.data())) {

    return false;
  }
  return true;
}
bool RootFileSystem::CreateRootDirectory() {
  return std::filesystem::create_directory(ROOTPATH.data());
}
void RootFileSystem::MountProcFolder() {
  mount("proc", "/proc", "proc", 0, NULL);
}
