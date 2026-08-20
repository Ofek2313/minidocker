#include "Config.h"
#include <string_view>

class RootFileSystem {
private:
  static constexpr std::string_view ROOTPATH = "/var/lib/minidocker";
  minidocker::FilePath logPath_ = "/var/log/minidocker.log";
  void CreateLogFile();

public:
  void DownloadAlpineEnvironment();
  bool SetRoot();
  bool IsRootFsInitialized();
  bool CreateRootDirectory();
  void MountProcFolder();

  void SetUpRootFileSystem();
};
