#pragma once
#include "FileDescriptor.h"
#include <span>
#include <sys/types.h>

class PipeHandler {
private:
  char readBuffer_[8192];
  FileDescriptor readDescriptor_;
  FileDescriptor writeDescriptor_;

private:
  bool IsOpen() const;

public:
  PipeHandler();

  void OpenPipe();
  ssize_t Write(const char *bytes, size_t length);
  ssize_t Read(std::span<char> buffer) const;

  const char *ReadBuffer() const;

  void CloseRead();
  void CloseWrite();
};
