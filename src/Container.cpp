#include "Container.h"
#include "Config.h"
#include "FileDescriptor.h"
#include "NamespaceConfig.h"
#include "RootFileSystem.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <sched.h>
#include <string>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

Container::Container() {}

size_t Container::GenerateHash() {
  std::string randomString = "";
  constexpr char letters[]{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
  std::srand(std::time(nullptr));
  for (int i{0}; i < 8; i++) {
    int random_num = std::rand() % sizeof(letters) + 1;
    randomString.push_back(letters[random_num]);
  }
  std::hash<std::string> stringHasher;

  size_t HashedString = stringHasher(randomString);
  return HashedString;
}

void Container::HandleErrors() {

  std::string errText = "";

  while (pipeHandler_.Read() > 0) {
    errText.append(pipeHandler_.ReadBuffer());
  }
  std::cerr << errText << std::endl;
  pipeHandler_.CloseRead();
}

int Container::child_function(void *args) {
  ChildArgs *childArgs = static_cast<ChildArgs *>(args);

  childArgs->pipeHandler_.CloseRead();

  try {
    RootFileSystem rfs;
    if (!rfs.IsRootFsInitialized()) {
      rfs.CreateRootDirectory();
      rfs.DownloadAlpineEnvironment();
    } // If Root FileSystem Not Initialized download and setup environment;

    rfs.setRoot(getpid());
    rfs.MountProcFolder();
    // auto memoryBuffer =
    // std::make_shared<std::vector<std::byte>>(1024 * 1024 * 1024);

    // std::fill_n(memoryBuffer->data(), 1024 * 1024 * 1024, std::byte{0xA});
    if (!childArgs->commands->empty()) {
      std::vector<char *> temp;

      for (auto &command : childArgs->commands.value()) {
        temp.push_back(command.data());
      }
      temp.push_back(nullptr);
      char **arg = temp.data();

      execvp(arg[0], arg);
    }

  } catch (const std::exception &ex) {
    childArgs->pipeHandler_.Write(ex.what(), std::strlen(ex.what()));

    _exit(1);
  }

  return 0;
}

void Container::Run(std::vector<std::string> &commands) {

  NamespaceConfig ns;
  ns.isolateMount().isolatePid();

  pipeHandler_.OpenPipe();
  minidocker::CgroupConfig config{100000, 1, "512M"};
  containerId_ = GenerateHash();
  cgroupManager_ = std::make_unique<CgroupManager>(containerId_, config);
  constexpr std::size_t stackSize = 1024 * 1024;
  auto stack = std::make_unique<std::byte[]>(stackSize);
  ChildArgs childArgs = {pipeHandler_, commands};
  pid_t pid =
      clone(child_function, stack.get() + stackSize, ns.getFlags(), &childArgs);
  cgroupManager_->AddProc(pid);
  pipeHandler_.CloseWrite();
  HandleErrors();
  std::cout << "Container Setup!" << std::endl;
  wait(NULL);
}
