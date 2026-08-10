
#include <curl/curl.h>
#include <curl/easy.h>
#include <filesystem>
#include <memory>
#include <string_view>
#include <zlib.h>
class Downloader {
private:
  struct GzFileDeleter {
    void operator()(gzFile gzFile) { gzclose(gzFile); }
  };

  struct CurlDeleter {
    void operator()(CURL *curl) { curl_easy_cleanup(curl); }
  };

  std::unique_ptr<CURL, CurlDeleter> handle;
  static size_t writeToStream(void *ptr, size_t size, size_t nmemb,
                              void *stream);

  using filePath = std::filesystem::path;

public:
  Downloader();
  void DownloadImage(std::string_view URL, std::string_view Path);
  void DeCompressArchive(std::string_view Path);
  void UnArchive();
};
