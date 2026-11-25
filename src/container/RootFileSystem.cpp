#include "container/RootFileSystem.h"
#include "Config.h"
#include "shared/Paths.h"
#include "tools/Downloader.h"
#include <cerrno>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <system_error>
#include <unistd.h>
#include <zlib.h>

void RootFileSystem::CreateLogFile() {

  // std::ofstream logFile("/var/log/minidocker.log");
  if (!std::filesystem::exists(logPath_)) {
    std::ofstream logFile(std::string(kLogFilePath).c_str());
    if (!logFile) {
      throw std::system_error(errno, std::generic_category(),
                              "Failed to create log file");
    }
    logFile.close();
  } else {
    std::cout << "Does Exsit" << std::endl;
  }
}

void RootFileSystem::MountImage() {

  minidocker::FilePath imagePath =
      std::string(kImagePathDefault) + "/" + imageName_ + "/upper";

  minidocker::OverlayFiles overlayFiles = {{imagePath},
                                           rootPath_ / "upper",
                                           rootPath_ / "work",
                                           rootPath_ / "merged"};
  std::cout << rootPath_ << std::endl;
  for (const auto &dir :
       {overlayFiles.merged, overlayFiles.upper, overlayFiles.work}) {

    std::filesystem::create_directories(dir);
  }
  // minidocker::checkErr(
  //    mount(imagePath.c_str(), rootPath_.c_str(), NULL, MS_BIND | MS_REC,
  //    NULL),
  //   "Mounting Image Failed");
  overlayFs_.emplace(overlayFiles, minidocker::CleanUpCode::Full);
  overlayFs_->MountOverlay();
}

bool RootFileSystem::SetRoot() {

  const std::string merged = std::string(rootPath_) + "/merged";
  // mount(rootPath_.c_str(), rootPath_.c_str(), NULL, MS_BIND, NULL);
  std::string oldRootPath = merged + "/oldroot";
  std::filesystem::create_directory(oldRootPath);

  minidocker::checkErr(
      syscall(SYS_pivot_root, merged.c_str(), oldRootPath.c_str()),
      "Failed To Pivot Root");

  minidocker::checkErr(chdir("/"), "Failed To Change Root Folder");

  minidocker::checkErr(umount2("/oldroot", MNT_DETACH),
                       "Failed To Unmount oldroot");
  std::filesystem::remove("/oldroot");
  mount(NULL, "/", NULL, MS_REMOUNT | MS_BIND | MS_RDONLY, NULL);
  return true;
}
void RootFileSystem::DownloadAlpineEnvironment() {

  Downloader downloader;
  std::string ALPINEURL =
      "https://dl-cdn.alpinelinux.org/alpine/latest-stable/releases/x86_64/"
      "alpine-minirootfs-3.24.0-x86_64.tar.gz";
  std::string PATH = "/tmp/alpine.tar.gz";

  downloader.DownloadImage(ALPINEURL, PATH);
  downloader.DeCompressArchive(PATH, "/var/lib/minidocker/bases");

  // error
}
bool RootFileSystem::IsRootFsInitialized() {

  return std::filesystem::is_directory(rootPath_);
}
bool RootFileSystem::CreateRootDirectory() {
  return std::filesystem::create_directory(rootPath_);
}
void RootFileSystem::MountProcFolder() {

  minidocker::checkErr(mount("proc", "/proc", "proc", 0, NULL),
                       "Proc Failed To Mount");
}
void RootFileSystem::SetUpRootFileSystem() {

  std::cout << "Testtttt" << '\n';
  minidocker::checkErr(
      mount(nullptr, "/", nullptr, MS_REC | MS_PRIVATE, nullptr),
      "Failed To Privitaize Mounts");
  std::cout << "Testtttt" << '\n';
  // CreateLogFile();
  try {

    if (!IsRootFsInitialized()) {
      CreateRootDirectory();
    }
  } catch (const std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }
  std::cout << "1" << '\n';
  MountImage();
  // DownloadAlpineEnvironment();
  std::cout << "2" << '\n';
  SetRoot();

  std::cout << "3" << '\n';
  MountProcFolder();
}
RootFileSystem::RootFileSystem(std::string containerId, std::string imageName)
    : containerId_{std::move(containerId)}, imageName_{std::move(imageName)},
      rootPath_(
          std::string(kRootPathDefault).append("/").append(containerId_)) {
  std::cout << containerId_ << '\n';
}
