#include "PipeHandler.h"
#include "FileDescriptor.h"
#include <fcntl.h>
#include <unistd.h>

PipeHandler::PipeHandler() {}

void PipeHandler::OpenPipe() {

  int pipeFd[2];
  if (pipe2(pipeFd, O_CLOEXEC) < 0) {
    // error
  }

  readDescriptor_ = FileDescriptor(pipeFd[0]);
  writeDescriptor_ = FileDescriptor(pipeFd[1]);

  isPipe_ = true;
}

bool PipeHandler::Write(const char *bytes, size_t length) {
  size_t bytesWritten = write(writeDescriptor_.get(), bytes, length);
  return bytesWritten > 0;
}

size_t PipeHandler::Read() {
  size_t bytesRead =
      read(readDescriptor_.get(), readBuffer_, sizeof(readBuffer_));
  return bytesRead;
}
const char *PipeHandler::ReadBuffer() const { return readBuffer_; }

void PipeHandler::CloseRead() { readDescriptor_.reset(); }
void PipeHandler::CloseWrite() { writeDescriptor_.reset(); }

PipeHandler::~PipeHandler() {}
