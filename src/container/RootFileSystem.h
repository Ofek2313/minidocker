#pragma once
#include "Config.h"
#include "tools/OverlayFs.h"
#include <array>
#include <optional>
#include <string_view>
#include <unordered_map>

class RootFileSystem {

private:
  std::optional<OverlayFs> overlayFs_;

  const std::string containerId_;
  minidocker::FilePath rootPath_;
  const std::string imageName_;
  minidocker::FilePath logPath_ = "/var/log/minidocker.log";
  void CreateLogFile();
  void MountImage();

public:
  RootFileSystem(std::string containerId, std::string imageName);

  void DownloadAlpineEnvironment();
  bool SetRoot();
  bool IsRootFsInitialized();
  bool CreateRootDirectory();
  void MountProcFolder();

  void SetUpRootFileSystem();
};
