
#include "Container.h"
#include "NamespaceConfig.h"

#include <filesystem>
#include <iostream>

#include <sched.h>
#include <string>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

int child_function(void *arg) {
  std::cout << getpid() << std::endl;

  using filepath = std::filesystem::path;
  filepath symPath = "/proc/" + std::to_string(getpid()) + "/exe";
  filepath procPath = std::filesystem::read_symlink(symPath);
  chroot(procPath.parent_path().c_str());

  return 0;
}

int main(int argc, char *argv[]) {

  Container container;
  container.InitContainer();
  return 0;
}
