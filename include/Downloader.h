
#include <curl/curl.h>
#include <curl/easy.h>
#include <memory>
#include <string_view>
class Downloader {
private:
  struct CurlDeleter {
    void operator()(CURL *curl) { curl_easy_cleanup(curl); }
  };

  std::unique_ptr<CURL, CurlDeleter> handle;
  static size_t writeToStream(void *ptr, size_t size, size_t nmemb,
                              void *stream);

public:
  Downloader();
  void DownloadImage(std::string_view URL, std::string_view Path);
};
