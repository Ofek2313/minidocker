#include "tools/PipeHandler.h"
#include "tools/FileDescriptor.h"
#include <cerrno>
#include <fcntl.h>
#include <system_error>
#include <unistd.h>

PipeHandler::PipeHandler() {}

void PipeHandler::OpenPipe() {

  int pipeFd[2];
  if (pipe2(pipeFd, O_CLOEXEC) < 0) {
    throw std::system_error(errno, std::generic_category(),
                            "Pipe Failed To Open");
  }

  readDescriptor_ = FileDescriptor(pipeFd[0]);
  writeDescriptor_ = FileDescriptor(pipeFd[1]);
}

ssize_t PipeHandler::Write(const char *bytes, size_t length) {
  size_t bytesWritten = write(writeDescriptor_.get(), bytes, length);

  if (bytesWritten < 0)
    throw std::system_error(errno, std::generic_category(), "Write Failed");

  return bytesWritten;
}

ssize_t PipeHandler::Read(std::span<char> buffer) const {
  ssize_t bytesRead =
      read(readDescriptor_.get(), buffer.data(), buffer.size_bytes());

  if (bytesRead < 0)
    throw std::system_error(errno, std::generic_category(), "Read Failed");
  return bytesRead;
}
const char *PipeHandler::ReadBuffer() const { return readBuffer_; }

void PipeHandler::CloseRead() { readDescriptor_.reset(); }
void PipeHandler::CloseWrite() { writeDescriptor_.reset(); }
