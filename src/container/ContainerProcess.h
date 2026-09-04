#pragma once
#include "Config.h"
#include <vector>

class ContainerProcess {

private:
  minidocker::ChildArgs *childArgs_;
  minidocker::ContainerConfig containerConfig_;
  const minidocker::FilePath logFilePath_ = "/var/log/minidocker.log";
  const minidocker::FilePath userBinaryPath_ = "/var/lib/minidocker/home/bin";

private:
  void ReDirectFileDescriptors();
  std::vector<char *> ParseCommands();
  void ExecuteCommands();
  void Detach();
  void ChangeWd(minidocker::FilePath workingDir);
  void CopyBinary(minidocker::FilePath binaryPath);

  void SetEnvVars();

public:
  ContainerProcess(minidocker::ChildArgs *childArgs,
                   minidocker::ContainerConfig containerConfig);
  void Run();
};
