#pragma once
#include "Config.h"
#include <vector>

class ContainerProcess {

private:
  minidocker::ChildArgs *childArgs_;
  const minidocker::FilePath logFilePath_ = "/var/log/minidocker.log";

private:
  void ReDirectFileDescriptors();
  std::vector<char *> ParseCommands();
  void ExecuteCommands();
  void Detach();

public:
  ContainerProcess(minidocker::ChildArgs *childArgs);
  void Run();
};
