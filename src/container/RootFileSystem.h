#include "Config.h"
#include <array>
#include <string_view>
#include <unordered_map>

class RootFileSystem {

private:
  struct OverlayFs {
    minidocker::FilePath lower;
    minidocker::FilePath upper;
    minidocker::FilePath work;
    minidocker::FilePath merged;

  } overlayFiles_;
  minidocker::FilePath rootPath_;
  const std::string imageName_;
  const std::string containerId_;
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
