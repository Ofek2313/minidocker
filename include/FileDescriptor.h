#pragma once
#include <unistd.h>

class FileDescriptor {
private:
  int fd_ = -1;

public:
  FileDescriptor() {};
  explicit FileDescriptor(int fd) noexcept : fd_{fd} {};

  FileDescriptor &operator=(const FileDescriptor &) = delete;
  FileDescriptor(const FileDescriptor &) = delete;

  FileDescriptor(FileDescriptor &&other) noexcept {
    if (fd_ >= 0)
      close(fd_);
    fd_ = other.fd_;
    other.fd_ = -1;
  }
  FileDescriptor &operator=(FileDescriptor &&other) noexcept {
    if (this != &other) {
      fd_ = other.fd_;
      other.fd_ = -1;
    }
    return *this;
  }
  ~FileDescriptor() { reset(); }

  void reset() {
    if (fd_ >= 0)
      close(fd_);
    fd_ = -1;
  }

  int get() { return fd_; }
};
