#include "container/ContainerProcess.h"
#include "Config.h"
#include "container/RootFileSystem.h"
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <ranges>
#include <stdexcept>
#include <system_error>
#include <thread>
#include <unistd.h>
#include <vector>

ContainerProcess::ContainerProcess(minidocker::ChildArgs *childArgs,
                                   minidocker::ContainerConfig containerConfig)
    : childArgs_{childArgs}, containerConfig_{containerConfig} {};

void ContainerProcess::ReDirectFileDescriptors() {

  FileDescriptor logFileDescriptor(
      open(logFilePath_.c_str(), O_WRONLY | O_APPEND | O_CREAT));

  for (int i{0}; i < 3; ++i) {
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

void ContainerProcess::ChangeWd(minidocker::FilePath workingDir) {
  if (!std::filesystem::is_directory(workingDir)) {
    std::cerr << "Unable to change working is_directory" << std::endl;
  }
  std::filesystem::current_path(workingDir);
}
void ContainerProcess::CopyBinary(minidocker::FilePath binaryPath) {
  if (!std::filesystem::is_directory(userBinaryPath_))
    std::filesystem::create_directory(userBinaryPath_);

  std::filesystem::copy(binaryPath, userBinaryPath_,
                        std::filesystem::copy_options::overwrite_existing);
}
void ContainerProcess::Run() {

  childArgs_->syncHandler.CloseWrite();
  childArgs_->pipeHandler.CloseRead();
  if (childArgs_->containerConfig.attachFlag)
    Detach();

  RootFileSystem rfs(containerConfig_.workingDirectory);
  rfs.SetUpRootFileSystem();

  if (childArgs_->containerConfig.attachFlag) {

    size_t bytes = childArgs_->syncHandler.Read();
    if (bytes < 0) {
      throw std::runtime_error("Error in reading pipe");
    }
  }
  std::cout << childArgs_->commands.data() << std::endl;
  // ChangeWd("/var");

  ExecuteCommands();

  // } catch (const std::exception exception) {

  //  childArgs_->pipeHandler.Write(exception.what(),
  //                               std::strlen(exception.what()));
  // exit(1);
  // }
}

void ContainerProcess::SetEnvVars() {

  if (!std::filesystem::exists("env.json")) {
    return;
  }

  std::ifstream f("env.json");
  nlohmann::json envData = nlohmann::json::parse(f);

  for (auto &var : envData.items()) {
    std::string value = var.value().is_string() ? var.value().get<std::string>()
                                                : var.value().dump();
    if (setenv(var.key().c_str(), value.c_str(), 0) == -1)
      throw std::system_error(errno, std::generic_category(),
                              "Failed to create env variable");
  }
}
