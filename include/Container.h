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
  std::string containerId_;

  struct FileDescriptorArgs {
    int readFd;
    int writeFd;
  };

  struct ChildArgs {
    PipeHandler &pipeHandler_;
    std::optional<std::vector<std::string>> commands;
  };

private:
  void InitCgroup(minidocker::CgroupConfig &config);
  void HandleErrors();
  size_t GenerateHash();

public:
  void Run(std::vector<std::string> &commands);
  Container();
};
