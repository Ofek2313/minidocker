#include "CgroupManager.h"
#include "Config.h"
#include "PipeHandler.h"
#include <memory>
#include <optional>
#include <sched.h>
#include <string>
#include <vector>

class Container {
private:
  static int child_function(void *args);

  std::unique_ptr<CgroupManager>
      cgroupManager_; // Constructor unknown at init so made it a ptr;
  PipeHandler pipeHandler_;
  size_t containerId_;

  minidocker::ContainerConfig containerConfig_;

  struct FileDescriptorArgs {
    int readFd;
    int writeFd;
  };

private:
  void InitCgroup(minidocker::CgroupConfig &config);
  void HandleErrors();

  size_t GenerateHash();
  void PrepareEnvironment();
  void CreateChildProcess(std::vector<std::string> &commands);

public:
  void Run(std::vector<std::string> &commands);
  void RunDetached(std::vector<std::string> &commands);
  void ConfigContainer();
  Container();
};
