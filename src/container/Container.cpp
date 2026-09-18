#include "container/Container.h"
#include "Config.h"
#include "container/ContainerProcess.h"
#include "container/NamespaceConfig.h"
#include "container/RootFileSystem.h"
#include "tools/FileDescriptor.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <sched.h>
#include <semaphore>
#include <string>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

Container::Container(minidocker::ContainerConfig config)
    : config_{std::move(config)} {}

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

  minidocker::ChildArgs *childArgs = static_cast<minidocker::ChildArgs *>(args);
  ContainerProcess process(childArgs, childArgs->containerConfig);
  process.Run();
  return 0;
}

void Container::PrepareEnvironment() {

  pipeHandler_.OpenPipe();

  containerId_ = GenerateHash();

  minidocker::FilePath rootPath =
      basePath_ / "containers" / std::to_string(containerId_);
  std::filesystem::create_directories(rootPath);

  cgroupManager_ =
      std::make_unique<CgroupManager>(containerId_, config_.cgroupConfig);
}

void Container::CreateChildProcess(std::vector<std::string> &commands) {

  NamespaceConfig ns;
  ns.isolateMount().isolatePid();

  constexpr std::size_t stackSize = 1024 * 1024;
  auto stack = std::make_unique<std::byte[]>(stackSize);
  minidocker::ChildArgs childArgs = {pipeHandler_, syncPipe_, config_, commands,
                                     containerId_};
  pid_t pid =
      clone(child_function, stack.get() + stackSize, ns.getFlags(), &childArgs);

  cgroupManager_->AddProc(pid);
  pipeHandler_.CloseWrite();
}

void Container::Run(std::vector<std::string> &commands) {

  PrepareEnvironment();

  CreateChildProcess(commands);

  if (config_.attachFlag) {

    syncPipe_.Write("W", 1);
    return;
  }

  HandleErrors();

  wait(NULL);
}
void Container::Init() {}
void Container::ConfigContainer() {}
