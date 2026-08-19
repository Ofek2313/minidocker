#pragma once
#include "FileDescriptor.h"

class PipeHandler {
private:
  char readBuffer_[8192];
  FileDescriptor readDescriptor_;
  FileDescriptor writeDescriptor_;
  bool isPipe_ = false;

private:
  bool IsOpen() const;

public:
  PipeHandler();
  ~PipeHandler();

  void OpenPipe();
  bool Write(const char *bytes, size_t length);
  size_t Read();

  const char *ReadBuffer() const;

  void CloseRead();
  void CloseWrite();
};
