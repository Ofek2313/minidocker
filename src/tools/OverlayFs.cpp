#include "OverlayFs.h"
#include <cerrno>
#include <filesystem>
#include <sched.h>
#include <sys/mount.h>
#include <system_error>
#include <utility>

OverlayFs::OverlayFs(minidocker::OverlayFiles overlayFiles,
                     minidocker::CleanUpCode code)
    : overlayFiles_(std::move(overlayFiles)), code_(std::move(code)) {}

void OverlayFs::CleanUp() {

  switch (code_) {
  case minidocker::CleanUpCode::None:
    break;
  case minidocker::CleanUpCode::Partial:
    std::filesystem::remove_all(overlayFiles_.merged);
    std::filesystem::remove_all(overlayFiles_.work);
    break;
  case minidocker::CleanUpCode::Full:
    std::filesystem::remove_all(overlayFiles_.merged);
    std::filesystem::remove_all(overlayFiles_.work);
    std::filesystem::remove_all(overlayFiles_.upper);
    break;
  }
}

void OverlayFs::MountOverlay() {
  if (mounted_)
    return;

  std::string options = "lowerdir=";

  for (auto itr = overlayFiles_.lower.begin(); itr != overlayFiles_.lower.end();
       ++itr) {

    options.append((*itr));

    if (itr != overlayFiles_.lower.end() - 1)
      options.append(":");
  }
  options.append(",upperdir=")
      .append(overlayFiles_.upper)
      .append(",workdir=")
      .append(overlayFiles_.work);

  int returnCode = mount("overlay", overlayFiles_.merged.c_str(), "overlay", 0,
                         options.c_str());
  if (returnCode == -1)
    throw std::system_error(errno, std::generic_category(),
                            "Overlay Mounting Failed");
  mounted_ = true;
}
void OverlayFs::UnMountOverlay() {
  if (!mounted_)
    return;
  int returnCode = umount(overlayFiles_.merged.c_str());
  if (returnCode == -1)
    throw std::system_error(errno, std::generic_category(),
                            "Overlay UnMounting Failed");
  mounted_ = false;
}

OverlayFs::OverlayFs(OverlayFs &&other)
    : overlayFiles_(std::move(other.overlayFiles_)) {
  mounted_ = std::exchange(other.mounted_, false);
}
OverlayFs &OverlayFs::operator=(OverlayFs &&other) {
  UnMountOverlay();
  overlayFiles_ = std::move(other.overlayFiles_);
  mounted_ = std::exchange(other.mounted_, false);
  return *this;
}
OverlayFs::~OverlayFs() { UnMountOverlay(); }
