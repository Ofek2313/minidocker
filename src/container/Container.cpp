#include "container/Container.h"
#include "Config.h"
#include "container/CgroupManager.h"
#include "container/ContainerProcess.h"
#include "container/NamespaceConfig.h"
#include "container/RootFileSystem.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <memory>
#include <sched.h>
#include <string>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <uuid.h>
#include <vector>

Container::~Container() = default;

std::string Container::UuidToText(const uuid_t id) {
  constexpr int kUuidStringSize = 37;
  char out[kUuidStringSize];
  uuid_unparse_lower(id, out);
  return std::string(out);
}

Container::Container(minidocker::ContainerConfig config)
    : config_{std::move(config)} {}

void Container::HandleErrors() {

  std::string errText = "";
  ssize_t bytes = 0;
  char buffer[8192];
  while ((bytes = pipeHandler_.Read(buffer)) > 0) {
    errText.append(pipeHandler_.ReadBuffer(),
                   bytes); // needs to know how many bytes read
  }
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

  uuid_generate(containerId_);

  minidocker::FilePath rootPath =
      basePath_ / "containers" / UuidToText(containerId_);

  std::filesystem::create_directories(rootPath);

  cgroupManager_ = std::make_unique<CgroupManager>(UuidToText(containerId_),
                                                   config_.cgroupConfig);
}

void Container::CreateChildProcess(std::vector<std::string> &commands) {

  NamespaceConfig ns;
  ns.isolateMount().isolatePid();

  constexpr std::size_t stackSize = 1024 * 1024;
  auto stack = std::make_unique<std::byte[]>(stackSize);
  minidocker::ChildArgs childArgs = {pipeHandler_, syncPipe_, config_, commands,
                                     UuidToText(containerId_)};
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
