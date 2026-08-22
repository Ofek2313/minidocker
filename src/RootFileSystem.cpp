#include "RootFileSystem.h"
#include "Downloader.h"
#include <cerrno>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <system_error>
#include <unistd.h>
#include <zlib.h>

void RootFileSystem::CreateLogFile() {

  // std::ofstream logFile("/var/log/minidocker.log");
  if (!std::filesystem::exists(logPath_)) {
    std::ofstream logFile("/var/log/minidocker.log");
    if (!logFile) {
      throw std::system_error(errno, std::generic_category(),
                              "Failed to create log file");
    }
    logFile.close();
  } else {
    std::cout << "Does Exsit" << std::endl;
  }
}

bool RootFileSystem::SetRoot() {

  mount(ROOTPATH.data(), ROOTPATH.data(), NULL, MS_BIND, NULL);
  std::string oldRootPath = std::string(ROOTPATH) + "/oldroot";
  std::filesystem::create_directory(oldRootPath);

  syscall(SYS_pivot_root, ROOTPATH.data(), oldRootPath.c_str());

  chdir("/");

  umount2("/oldroot", MNT_DETACH);
  std::filesystem::remove("/oldroot");
  return true;
}

void RootFileSystem::DownloadAlpineEnvironment() {

  Downloader downloader;
  std::string ALPINEURL =
      "https://dl-cdn.alpinelinux.org/alpine/latest-stable/releases/x86_64/"
      "alpine-minirootfs-3.24.0-x86_64.tar.gz";
  std::string PATH = "/tmp/alpine.tar.gz";

  downloader.DownloadImage(ALPINEURL, PATH);
  downloader.DeCompressArchive(PATH, "/var/lib/minidocker");

  // error
}
bool RootFileSystem::IsRootFsInitialized() {

  return std::filesystem::is_directory(ROOTPATH.data());
}
bool RootFileSystem::CreateRootDirectory() {
  return std::filesystem::create_directory(ROOTPATH.data());
}
void RootFileSystem::MountProcFolder() {
  if (mount("proc", "/proc", "proc", 0, NULL) == -1) {
    throw std::system_error(errno, std::generic_category(),
                            "Proc Folder Failed To Mount");
  }
}
void RootFileSystem::SetUpRootFileSystem() {

  mount(nullptr, "/", nullptr, MS_REC | MS_PRIVATE, nullptr);
  CreateLogFile();
  if (!IsRootFsInitialized()) {
    CreateRootDirectory();
    DownloadAlpineEnvironment();
  }
  SetRoot();
  MountProcFolder();
}
