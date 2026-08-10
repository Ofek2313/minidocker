#include "Downloader.h"

#include <archive.h>
#include <archive_entry.h>
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
void Downloader::DeCompressArchive(std::string_view Path) {
  struct archive *a;
  struct archive *write;
  struct archive_entry *entry;
  char buffer[8192];
  a = archive_read_new();
  write = archive_write_disk_new();
  int flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM |
              ARCHIVE_EXTRACT_OWNER | ARCHIVE_EXTRACT_UNLINK |
              ARCHIVE_EXTRACT_SECURE_NODOTDOT;
  archive_write_disk_set_options(write, flags);
  archive_write_disk_set_standard_lookup(write);
  int bytes = 0;

  archive_read_support_filter_gzip(a);
  archive_read_support_format_tar(a);
  archive_read_open_filename(a, "/tmp/alpine.tar.gz", 10240);
  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {

    filePath currentPath = archive_entry_pathname(entry);
    archive_write_header(write, entry);
    std::cout << currentPath << std::endl;
    std::ofstream outFile(currentPath, std::ios::binary);

    while ((bytes = archive_read_data(a, buffer, sizeof(buffer))) > 0) {
      // write to file
      archive_write_data(write, buffer, bytes);
    }

    archive_write_finish_entry(write);
    // go over to next header
  }

  archive_read_free(a);
  archive_write_free(write);
}
