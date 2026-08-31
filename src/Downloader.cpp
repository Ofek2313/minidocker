#include "Downloader.h"
#include <archive.h>
#include <archive_entry.h>
#include <cerrno>
#include <cstdlib>
#include <curl/curl.h>
#include <curl/easy.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <zconf.h>

Downloader::Downloader() : handle(curl_easy_init()) {
  if (!handle) {
    throw std::runtime_error("Curl Failed To Init");
  }
}
size_t Downloader::writeToStream(void *ptr, size_t size, size_t nmemb,
                                 void *stream) {
  auto *file = static_cast<std::ofstream *>(stream);
  file->write(static_cast<char *>(ptr), nmemb * size);
  return nmemb * size;
}

void Downloader::DownloadImage(const std::string &URL,
                               const minidocker::FilePath &Path) {

  std::ofstream file(Path, std::ios::binary);
  if (!file.is_open()) {
    throw std::system_error(errno, std::generic_category(),
                            "Destination File Failed To Open: " +
                                Path.string());
  }
  curl_easy_setopt(handle.get(), CURLOPT_URL, URL.c_str());
  curl_easy_setopt(handle.get(), CURLOPT_WRITEFUNCTION, writeToStream);
  curl_easy_setopt(handle.get(), CURLOPT_WRITEDATA, &file);
  curl_easy_setopt(handle.get(), CURLOPT_FOLLOWLOCATION, 1);
  CURLcode result = curl_easy_perform(handle.get());
  if (result != CURLE_OK) {
    throw std::runtime_error(std::string("Download Failed: ") +
                             curl_easy_strerror(result));
  }
  std::cout << "Downloaded" << std::endl;
}
void Downloader::DeCompressArchive(const minidocker::FilePath &Path,
                                   const minidocker::FilePath &Dest) {

  if (!std::filesystem::exists(Path) || !std::filesystem::exists(Dest)) {
    throw std::runtime_error("File Do Not Exist");
  }

  std::unique_ptr<archive, ArchiveReadDeleter> a(archive_read_new());
  std::unique_ptr<archive, ArchiveWriteDeleter> write(archive_write_disk_new());
  struct archive_entry *entry;

  char buffer[8192];

  int flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM |
              ARCHIVE_EXTRACT_OWNER | ARCHIVE_EXTRACT_UNLINK |
              ARCHIVE_EXTRACT_SECURE_NODOTDOT;
  archive_write_disk_set_options(write.get(), flags);
  archive_write_disk_set_standard_lookup(write.get());
  int bytes = 0;

  archive_read_support_filter_gzip(a.get());
  archive_read_support_format_tar(a.get());

  if (archive_read_open_filename(a.get(), Path.c_str(), 10240) ==
      ARCHIVE_FATAL) {
    throw std::runtime_error(std::string("failed to open fail") +
                             archive_error_string(a.get()));
  }
  while (archive_read_next_header(a.get(), &entry) == ARCHIVE_OK) {

    minidocker::FilePath currentPath = archive_entry_pathname(entry);
    minidocker::FilePath systemPath = (Dest / currentPath).lexically_normal();
    archive_entry_set_pathname(entry, systemPath.c_str());

    archive_write_header(write.get(), entry);

    while ((bytes = archive_read_data(a.get(), buffer, sizeof(buffer))) > 0) {
      // write to file
      int written = archive_write_data(write.get(), buffer, bytes);
      if (written < 0) {
        throw std::runtime_error(std::string("File Corrupted") +
                                 archive_error_string(a.get()));
      }
    }

    archive_write_finish_entry(write.get());
    // go over to next header
  }
  std::cout << "Flag2" << std::endl;
}
