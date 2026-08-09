#include "RootFileSystem.h"
#include <filesystem>
#include <unistd.h>

bool RootFileSystem::setRoot(int pid) {

  using filepath = std::filesystem::path;
  filepath symPath = "/proc/" + std::to_string(getpid()) + "/exe";
  filepath procPath = std::filesystem::read_symlink(symPath);
  int status = chroot(procPath.parent_path().c_str());
  return (status == 0) ? true : false;
}
