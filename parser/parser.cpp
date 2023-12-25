#include "parser.h"

uint32_t to_int(char* num) {
    uint32_t ans = 0;
    for (uint32_t i = 0; i < strlen(num); i++) {
        ans = ans * 10 + (num[i] - '0');
    }

    return ans;
}

uint32_t CalculateAddBits(uint32_t block) {
    uint32_t predict_size = block + log2(block) + 1;

    if (CheckOkay(predict_size, block)) {
        return log2(block) + 1;
    }

    return log2(block) + 2;
}

void FindRestInfo(uint32_t i, uint32_t argc, char** argv, ArchiveInfo& arguments) {
    for (size_t j = i + 1; j < argc; j++) {
        if (argv[j][0] != '-') {
            arguments.files.push_back(argv[j]);
            continue;
        }
        if (strcmp(argv[j], "-f") == 0) {
            j++;
            arguments.archive_name = argv[j];
        } else if (strncmp(argv[j], "--file=", 7) == 0) {
            for (uint32_t l = 7; l < strlen(argv[j]); l++) {
                arguments.archive_name += argv[j][l];
            }
        } else if (strcmp(argv[j], "--info") == 0 || strcmp(argv[j], "-i") == 0) {
            arguments.cnt_bits_in_block = to_int(argv[++j]);
        }
    }
}

void ParseArguments(uint32_t argc, char** argv, ArchiveInfo& arguments) {
    for (uint32_t i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-A") == 0 || strcmp(argv[i], "--concatenate") == 0) {
            arguments.is_concatenate = true;
            bool found_arch = false;
            for (++i; i < argc; i++) {
                if (strcmp(argv[i], "-f") == 0) {
                    ++i;
                    arguments.first_archive_name = argv[i];
                } else if (argv[i][0] == '-') {
                    if (strncmp(argv[i], "--file=", 7) == 0) {
                        for (int l = 7; l < strlen(argv[i]); l++) {
                            arguments.first_archive_name += argv[i][l];
                        }
                    }
                } else {
                    if (!found_arch) {
                        arguments.archive_name = argv[i];
                        found_arch = true;
                    } else {
                        arguments.second_archive_name = argv[i];
                    }
                }
            }
            arguments.cnt_add_bits = CalculateAddBits(arguments.cnt_bits_in_block);
            return;
        }

        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--create") == 0) {
            arguments.is_create = true;
            FindRestInfo(i, argc, argv, arguments);
            arguments.cnt_add_bits = CalculateAddBits(arguments.cnt_bits_in_block);
            return;
        }

        if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--append") == 0) {
            FindRestInfo(i, argc, argv, arguments);
            arguments.is_append = true;
            arguments.cnt_add_bits = CalculateAddBits(arguments.cnt_bits_in_block);
            return;
        }

        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--delete") == 0) {
            FindRestInfo(i, argc, argv, arguments);
            arguments.is_delete = true;
            arguments.cnt_add_bits = CalculateAddBits(arguments.cnt_bits_in_block);
            return;
        }

        if (strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--extract") == 0) {
            arguments.is_extract = true;
            FindRestInfo(i, argc, argv, arguments);
            if (arguments.files.empty()) {
                arguments.is_all_files = true;
            }
            arguments.cnt_add_bits = CalculateAddBits(arguments.cnt_bits_in_block);
            return;
        }

        if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--list") == 0) {
            FindRestInfo(i, argc, argv, arguments);
            arguments.is_list = true;
            arguments.cnt_add_bits = CalculateAddBits(arguments.cnt_bits_in_block);
            return;
        }
    }
}

bool CheckOkay(uint32_t len, uint32_t orig) {
    uint32_t cnt_add_bits = 0;
    for (uint32_t i = 0; i < 30; i++) {
        if ((1 << i) > len) break;
        cnt_add_bits++;
    }
    if (len - cnt_add_bits != orig) {
        return false;
    }

    return true;
}
