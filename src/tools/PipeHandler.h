#pragma once
#include "FileDescriptor.h"
#include <memory>
#include <span>
#include <sys/types.h>

class PipeHandler {

private:
  char readBuffer_[8192];
  std::shared_ptr<FileDescriptor> readDescriptor_;
  std::shared_ptr<FileDescriptor> writeDescriptor_;

private:
  bool IsOpen() const;

public:
  PipeHandler();
  PipeHandler(const PipeHandler &other) = default;
  PipeHandler &operator=(const PipeHandler &other) = default;
  PipeHandler(PipeHandler &&other) = default;
  PipeHandler &operator=(PipeHandler &&other) = default;

  void OpenPipe();
  ssize_t Write(const char *bytes, size_t length);
  ssize_t Read(std::span<char> buffer) const;

  const char *ReadBuffer() const;

  void CloseRead();
  void CloseWrite();
};
