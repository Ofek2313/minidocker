#include "Config.h"
#include <string_view>

class RootFileSystem {
private:
  const minidocker::FilePath rootPath_;
  const minidocker::FilePath imagePath_;
  minidocker::FilePath logPath_ = "/var/log/minidocker.log";
  void CreateLogFile();

public:
  RootFileSystem(minidocker::FilePath rootPath, minidocker::FilePath imagePath);

  void DownloadAlpineEnvironment();
  bool SetRoot();
  bool IsRootFsInitialized();
  bool CreateRootDirectory();
  void MountProcFolder();

  void SetUpRootFileSystem();
};
