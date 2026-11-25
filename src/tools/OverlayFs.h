
#include <filesystem>
#include <vector>

namespace minidocker {
struct OverlayFiles {
  std::vector<std::filesystem::path> lower;
  std::filesystem::path upper;
  std::filesystem::path work;
  std::filesystem::path merged;
};

enum class CleanUpCode { None, Partial, Full };
} // namespace minidocker
class OverlayFs {

private:
  minidocker::OverlayFiles overlayFiles_;

  minidocker::CleanUpCode code_;
  bool mounted_ = false;

  void CleanUp();

public:
  explicit OverlayFs(minidocker::OverlayFiles overlayFiles,
                     minidocker::CleanUpCode code);

  OverlayFs(const OverlayFs &) = delete;
  OverlayFs &operator=(const OverlayFs &) = delete;

  OverlayFs(OverlayFs &&other);
  OverlayFs &operator=(OverlayFs &&other);

  ~OverlayFs();
  void MountOverlay();
  void UnMountOverlay();
};
