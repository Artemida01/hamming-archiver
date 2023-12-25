#pragma once

#include <fstream>
#include <cstdio>
#include "parser.h"
#include "hamming.h"

struct HeaderInfo {
  std::string file_name;
  uint32_t orig_file_size = 0;
  uint32_t before_hamming_block_size = 0;
};

void CreateArchive(ArchiveInfo& archive);

std::vector<std::string> GetArchiveFiles(ArchiveInfo& archive);

void ExtractArchive(ArchiveInfo& archive);

void AddFileToArchive(ArchiveInfo& archive, char* file_name);

void DeleteFile(ArchiveInfo& archive, char* file_name);

void MergeArchives(ArchiveInfo& archive);