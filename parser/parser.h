#pragma once
#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>

struct ArchiveInfo {
  std::string archive_name;

  char* first_archive_name;
  char* second_archive_name;

  std::vector<char*> files;

  uint32_t cnt_bits_in_block = 16;
  uint32_t cnt_add_bits = 21;

  bool is_all_files = false;
  bool is_create = false;
  bool is_list = false;
  bool is_extract = false;
  bool is_append = false;
  bool is_delete = false;
  bool is_concatenate = false;
};

void ParseArguments(uint32_t argc, char** argv, ArchiveInfo& arguments);

void CalculateAddBits(ArchiveInfo& arguments);

void FindRestInfo(uint32_t i, uint32_t argc, char** argv, ArchiveInfo& arguments);

bool CheckOkay(uint32_t len, uint32_t orig);

uint32_t CalculateAddBits(uint32_t block);