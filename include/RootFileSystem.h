#include <string_view>

class RootFileSystem {
private:
  static constexpr std::string_view ROOTPATH = "~/alpine";

public:
  void DownloadAlpineEnvironment();
  bool setRoot(int pid);
};
