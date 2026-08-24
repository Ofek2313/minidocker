#include "Container.h"
#include "Config.h"

#include "ContainerProcess.h"
#include "FileDescriptor.h"
#include "NamespaceConfig.h"
#include "RootFileSystem.h"
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

  minidocker::ChildArgs *childArgs = static_cast<minidocker::ChildArgs *>(args);
  ContainerProcess process(childArgs, childArgs->containerConfig);
  process.Run();
  return 0;
}

void Container::PrepareEnvironment() {

  pipeHandler_.OpenPipe();

  containerId_ = GenerateHash();
  minidocker::CgroupConfig config{100000, 1, "512M"};
  containerConfig_ = minidocker::ContainerConfig{config, "Test", "/", false};
  cgroupManager_ = std::make_unique<CgroupManager>(containerId_, config);
}

void Container::CreateChildProcess(std::vector<std::string> &commands) {

  NamespaceConfig ns;
  ns.isolateMount().isolatePid();

  constexpr std::size_t stackSize = 1024 * 1024;
  auto stack = std::make_unique<std::byte[]>(stackSize);
  minidocker::ChildArgs childArgs = {pipeHandler_, containerConfig_, commands};
  pid_t pid =
      clone(child_function, stack.get() + stackSize, ns.getFlags(), &childArgs);
  std::cout << pid << std::endl;
  cgroupManager_->AddProc(pid);
  pipeHandler_.CloseWrite();
}

void Container::Run(std::vector<std::string> &commands) {

  containerConfig_.attachFlag = true;
  PrepareEnvironment();

  CreateChildProcess(commands);

  if (containerConfig_.attachFlag)
    return;

  HandleErrors();

  wait(NULL);
}

void Container::ConfigContainer() {
  containerConfig_.attachFlag = false;
  containerConfig_.containerName = "test";
}
