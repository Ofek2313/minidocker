#include "ContainerProcess.h"
#include "RootFileSystem.h"
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <system_error>
#include <unistd.h>
#include <vector>

ContainerProcess::ContainerProcess(minidocker::ChildArgs *childArgs)
    : childArgs_{childArgs} {};

void ContainerProcess::ReDirectFileDescriptors() {

  FileDescriptor logFileDescriptor(
      open(logFilePath_.c_str(), O_WRONLY | O_APPEND));

  for (int i{}; i < 3; ++i) {
    if (dup2(logFileDescriptor.get(), i) == -1) {
      throw std::system_error(errno, std::generic_category(),
                              "Unable to allocate new file descriptor");
    }
  }
}

std::vector<char *> ContainerProcess::ParseCommands() {

  std::vector<char *> parsedCommands;

  for (auto &command : childArgs_->commands) {
    parsedCommands.push_back(command.data());
  }
  parsedCommands.push_back(nullptr);

  return parsedCommands;
}

void ContainerProcess::ExecuteCommands() {
  if (!childArgs_->commands.empty()) {
    std::vector<char *> commands = ParseCommands();
    if (execvp(commands[0], commands.data()) == -1) {
      throw std::system_error(errno, std::generic_category(),
                              "Command Execution Failed");
    }
  }
}
void ContainerProcess::Detach() {

  setsid();
  ReDirectFileDescriptors();
}

void ContainerProcess::Run() {

  childArgs_->pipeHandler.CloseRead();
  // Detach();

  RootFileSystem rootFileSystem;
  rootFileSystem.SetUpRootFileSystem();
  std::cout << childArgs_->commands.data() << std::endl;
  ExecuteCommands();

  // } catch (const std::exception exception) {

  //  childArgs_->pipeHandler.Write(exception.what(),
  //                               std::strlen(exception.what()));
  // exit(1);
  // }
}
