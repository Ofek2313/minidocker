#include "Downloader.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

Downloader::Downloader() : handle(curl_easy_init()) {
  if (!handle) {
    std::cerr << "Init Failed" << "\n";
  }
}
size_t Downloader::writeToStream(void *ptr, size_t size, size_t nmemb,
                                 void *stream) {
  auto *file = static_cast<std::ofstream *>(stream);
  file->write(static_cast<char *>(ptr), nmemb * size);
  return nmemb * size;
}

void Downloader::DownloadImage(std::string_view URL, std::string_view Path) {
  std::ofstream file(Path.data(), std::ios::binary);
  curl_easy_setopt(handle.get(), CURLOPT_URL, URL.data());
  curl_easy_setopt(handle.get(), CURLOPT_WRITEFUNCTION, writeToStream);
  curl_easy_setopt(handle.get(), CURLOPT_WRITEDATA, &file);
  curl_easy_setopt(handle.get(), CURLOPT_FOLLOWLOCATION, 1);
  CURLcode result = curl_easy_perform(handle.get());
  if (result != CURLE_OK) {
    std::cerr << "download failed" << "\n";
  }
}
