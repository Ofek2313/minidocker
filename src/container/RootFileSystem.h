#include "Config.h"
#include <string_view>

class RootFileSystem {
private:
  minidocker::FilePath rootPath_;
  const std::string imageName_;
  const size_t containerId_;
  minidocker::FilePath logPath_ = "/var/log/minidocker.log";
  void CreateLogFile();
  void MountImage();

public:
  RootFileSystem(size_t containerId, std::string imageName);

  void DownloadAlpineEnvironment();
  bool SetRoot();
  bool IsRootFsInitialized();
  bool CreateRootDirectory();
  void MountProcFolder();

  void SetUpRootFileSystem();
};
