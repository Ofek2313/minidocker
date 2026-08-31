#include "Config.h"
#include <string_view>

class RootFileSystem {
private:
  const minidocker::FilePath rootPath_;
  minidocker::FilePath logPath_ = "/var/log/minidocker.log";
  void CreateLogFile();

public:
  RootFileSystem(minidocker::FilePath rootPath);

  void DownloadAlpineEnvironment();
  bool SetRoot();
  bool IsRootFsInitialized();
  bool CreateRootDirectory();
  void MountProcFolder();

  void SetUpRootFileSystem();
};
