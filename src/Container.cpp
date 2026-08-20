#include "Container.h"
#include "Config.h"
#include "FileDescriptor.h"
#include "NamespaceConfig.h"
#include "RootFileSystem.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <optional>
#include <sched.h>
#include <string>
#include <sys/mount.h>
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
    int random_num = std::rand() % sizeof(letters);
    randomString.push_back(letters[random_num]);
  }
  std::hash<std::string> stringHasher;

  size_t HashedString = stringHasher(randomString);
  return HashedString;
}

void Container::HandleErrors() {

  std::string errText = "";
  size_t bytes = 0;
  while ((bytes = pipeHandler_.Read()) > 0) {
    errText.append(pipeHandler_.ReadBuffer(), bytes);
  }
  std::cerr << errText << std::endl;
  pipeHandler_.CloseRead();
}

int Container::child_function(void *args) {

  ChildArgs *childArgs = static_cast<ChildArgs *>(args);

  if (minidocker::containerConfig.attachFlag) {

    FileDescriptor logFileDescriptor(
        open("/var/log/minidocker.log", O_WRONLY | O_APPEND));
    setsid();
    dup2(logFileDescriptor.get(), STDOUT_FILENO);
    dup2(logFileDescriptor.get(), STDIN_FILENO);
    dup2(logFileDescriptor.get(), STDERR_FILENO);
  }
  std::cout << "Error" << '\n';

  // std::this_thread::sleep_for(std::chrono::seconds(10));
  try {

    childArgs->pipeHandler_.CloseRead();
    mount(nullptr, "/", nullptr, MS_REC | MS_PRIVATE, nullptr);

    RootFileSystem rfs;
    rfs.SetUpRootFileSystem();
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

void Container::PrepareEnvironment() {

  pipeHandler_.OpenPipe();

  containerId_ = GenerateHash();
  minidocker::CgroupConfig config{100000, 1, "512M"};
  cgroupManager_ = std::make_unique<CgroupManager>(containerId_, config);
}

void Container::CreateChildProcess(std::vector<std::string> &commands) {

  NamespaceConfig ns;
  ns.isolateMount().isolatePid();

  constexpr std::size_t stackSize = 1024 * 1024;
  auto stack = std::make_unique<std::byte[]>(stackSize);
  ChildArgs childArgs = {pipeHandler_, commands};
  pid_t pid =
      clone(child_function, stack.get() + stackSize, ns.getFlags(), &childArgs);
  cgroupManager_->AddProc(pid);
  pipeHandler_.CloseWrite();
}

void Container::Run(std::vector<std::string> &commands) {

  minidocker::containerConfig.attachFlag = true;
  PrepareEnvironment();

  CreateChildProcess(commands);

  if (minidocker::containerConfig.attachFlag)
    return;

  HandleErrors();

  wait(NULL);
}

void Container::ConfigContainer() {
  minidocker::containerConfig.attachFlag = false;
  minidocker::containerConfig.containerName = "test";
}
