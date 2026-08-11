#include <string_view>

class RootFileSystem {
private:
  static constexpr std::string_view ROOTPATH = "/var/lib/minidocker";

public:
  void DownloadAlpineEnvironment();
  bool setRoot(int pid);
  bool IsRootFsInitialized();
  bool CreateRootDirectory();
  void MountProcFolder();
};
