
#include <archive.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <filesystem>
#include <memory>
#include <zlib.h>

enum class DownloadStatus {
  Success,
  DownloadFailed,
  FileNotFound,
  FileFailedToOpen,
  FileCorrupted,

};

class Downloader {
private:
  struct GzFileDeleter {
    void operator()(gzFile gzFile) { gzclose(gzFile); }
  };

  struct CurlDeleter {
    void operator()(CURL *curl) { curl_easy_cleanup(curl); }
  };
  struct ArchiveReadDeleter {
    void operator()(archive *a) { archive_read_free(a); }
  };
  struct ArchiveWriteDeleter {
    void operator()(archive *w) { archive_write_free(w); }
  };

  std::unique_ptr<CURL, CurlDeleter> handle;
  static size_t writeToStream(void *ptr, size_t size, size_t nmemb,
                              void *stream);

  using filePath = std::filesystem::path;

public:
  Downloader();
  DownloadStatus DownloadImage(const std::string &URL, const filePath &Path);
  DownloadStatus DeCompressArchive(const filePath &Path, const filePath &Dest);
  void UnArchive();
};
