#pragma once
#include <unistd.h>
#include <utility>

class FileDescriptor {
private:
  int fd_ = -1;

public:
  FileDescriptor() {};
  explicit FileDescriptor(int fd) noexcept : fd_{fd} {}

  FileDescriptor &operator=(const FileDescriptor &) = delete;
  FileDescriptor(const FileDescriptor &) = delete;

  FileDescriptor(FileDescriptor &&other) noexcept { fd_ = other.release(); }
  FileDescriptor &operator=(FileDescriptor &&other) noexcept {
    if (this != &other) {
      if (fd_ >= 0)
        close(fd_);
      fd_ = other.release();
    }
    return *this;
  }
  ~FileDescriptor() { reset(); }

  void reset() noexcept {
    if (fd_ >= 0)
      close(fd_);
    fd_ = -1;
  }

  [[nodiscard]] int get() const noexcept { return fd_; }

  [[nodiscard]] int release() noexcept { return std::exchange(fd_, -1); }
};
