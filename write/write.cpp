#include "write.h"

uint32_t GetFileSize(std::ifstream& file_from) {
    std::streampos file_size = 0;
    file_from.seekg(0, std::ios::end);
    file_size = file_from.tellg();
    file_from.seekg(0, std::ios::beg);

    return file_size;
}

void MakeHeader(std::ofstream& file_to, char* file_name, std::ifstream& file_from, uint32_t cnt_bits_in_block) {
    uint32_t n = strlen(file_name);
    for (uint32_t i = 0; i < n; i++) { // name
        file_to << file_name[i];
    }

    std::streampos file_size = GetFileSize(file_from); // size
    file_to << " " << file_size;

    file_to << " " << cnt_bits_in_block << '\n'; // cnt bits in block
}

void ReadHeader(std::ifstream& file, HeaderInfo& cur_file) {
    char cur;
    while (!file.eof()) {
        file.get(cur);
        if (cur == ' ' || file.eof()) {
            break;
        }
        cur_file.file_name += cur;
    }

    if (file.eof()) {
        return;
    }

    while (!file.eof()) {
        file.get(cur);
        if (cur == ' ' || file.eof()) {
            break;
        }
        cur_file.orig_file_size = cur_file.orig_file_size * 10 + (cur - '0');
    }

    while (!file.eof()) {
        file.get(cur);
        if (cur == '\n' || file.eof()) {
            break;
        }

        cur_file.before_hamming_block_size = cur_file.before_hamming_block_size * 10 + (cur - '0');
    }
}

void Write(std::ofstream& file, bool* bits, uint32_t n, uint32_t cnt_add_bits) {
    std::vector<char> code = ToHamming(bits, n, cnt_add_bits);
    for (size_t i = 0; i < code.size(); i++) {
        file << code[i];
    }
}

uint32_t CalculateRealBlockSize(uint32_t block) {
    return block + CalculateAddBits(block);
}

uint32_t CalculateRealFileSize(uint32_t size, uint32_t block) {
    uint32_t real_block_size = CalculateRealBlockSize(block);

    uint32_t real_file_size = (size * 8 + block - 1) / block;
    real_file_size *= (real_block_size + 7) / 8;

    return real_file_size;
}

void AddFileToArchive(ArchiveInfo& archive, char* file_name) {
    std::ifstream file;
    file.open(file_name, std::ios::binary);
    if (!file.is_open()) {
        std::string s = "Can't open file ";
        for (size_t i = 0; i < strlen(file_name); i++) {
            s += file_name[i];
        }
        ThrowBug(s);
    }

    std::ofstream arch;
    arch.open(archive.archive_name, std::ios::app);
    if (!arch.is_open()) {
        std::string s = "Can't open archive";
        ThrowBug(s);
    }

    MakeHeader(arch, file_name, file, archive.cnt_bits_in_block);

    uint32_t cur_poz = 0;
    bool bits[archive.cnt_bits_in_block];
    SetZeros(bits, archive.cnt_bits_in_block);

    char cur_symbol;
    while (!file.eof()) {
        file.get(cur_symbol);
        if (file.eof()) {
            break;
        }
        for (uint32_t bit = 0; bit < 8; bit++) {
            bits[cur_poz++] = (cur_symbol >> bit) & 1;
            if (cur_poz == archive.cnt_bits_in_block) {
                Write(arch, bits, archive.cnt_bits_in_block, archive.cnt_add_bits);
                cur_poz = 0;
                SetZeros(bits, archive.cnt_bits_in_block);
            }
        }
    }

    if (cur_poz != 0) {
        Write(arch, bits, archive.cnt_bits_in_block, archive.cnt_add_bits);
    }
    arch << '\n';

    arch.close();
    file.close();

    std::remove(file_name); // delete file after add it to archive
}

void CreateArchive(ArchiveInfo& archive) {
    uint32_t cnt_files = archive.files.size();
    for (size_t i = 0; i < cnt_files; i++) {
        AddFileToArchive(archive, archive.files[i]);
    }
}

std::vector<std::string> GetArchiveFiles(ArchiveInfo& archive) {
    std::ifstream file;
    file.open(archive.archive_name);

    if (!file.is_open()) {
        std::string s = "Can't open archive";
        ThrowBug(s);
    }

    std::vector<std::string> ans;

    while (!file.eof()) {
        HeaderInfo cur_file;
        ReadHeader(file, cur_file);

        if (file.eof()) {
            break;
        }

        ans.push_back(cur_file.file_name);

        uint32_t hamming_file_size = CalculateRealFileSize(cur_file.orig_file_size, cur_file.before_hamming_block_size);
        file.seekg(hamming_file_size + 1, std::ios_base::cur);
    }

    return ans;
}

void ExtractArchive(ArchiveInfo& archive) {
    std::ifstream file;
    file.open(archive.archive_name);
    if (!file.is_open()) {
        std::string s = "Can't open archive";
        ThrowBug(s);
    }

    std::string temp_file_name = "$temp$.haf";
    std::ofstream temp_file;
    if (!archive.is_all_files) {
        temp_file.open(temp_file_name);
    }

    char cur = 0;
    bool has_one_file = false;
    while (!file.eof()) {
        HeaderInfo cur_file;
        ReadHeader(file, cur_file);

        if (file.eof()) {
            break;
        }

        uint32_t after_hamming_block_size = CalculateRealBlockSize(cur_file.before_hamming_block_size);
        uint32_t hamming_file_size = CalculateRealFileSize(cur_file.orig_file_size, cur_file.before_hamming_block_size);

        bool keep_in_archive = (count(archive.files.begin(), archive.files.end(), cur_file.file_name) == 0) &&
            !archive.is_all_files;
        if (keep_in_archive) {
            temp_file << cur_file.file_name << " " << cur_file.orig_file_size << " "
                      << cur_file.before_hamming_block_size << '\n';
            for (uint64_t i = 0; i < hamming_file_size; i++) {
                file.get(cur);
                temp_file << cur;
            }
            file.get(cur);
            temp_file << cur;
            has_one_file = true;
            continue;
        }

        std::ofstream file_to;
        file_to.open(cur_file.file_name);
        if (!file.is_open()) {
            std::string s = "Can't open ";
            for (size_t i = 0; i < cur_file.file_name.size(); i++) {
                s += cur_file.file_name[i];
            }
            ThrowBug(s);
        }

        uint32_t cur_poz = 0;
        bool bits[after_hamming_block_size];
        SetZeros(bits, after_hamming_block_size);

        char last_char = 0;
        uint32_t poz_last_bit = 0;
        uint32_t already_taken = 0;
        for (uint32_t i = 0; i < hamming_file_size; i++) {
            file.get(cur);
            for (uint32_t bit = 0; bit < 8; bit++) {
                bits[cur_poz++] = ((cur >> bit) & 1);

                if (cur_poz == after_hamming_block_size) {
                    std::vector<char> orig =
                        FromHamming(bits,
                                    after_hamming_block_size,
                                    after_hamming_block_size - cur_file.before_hamming_block_size);
                    for (size_t j = 0; j < orig.size() - 1 && already_taken < cur_file.orig_file_size; j++) {
                        for (uint32_t k = 0; k < 8 && already_taken < cur_file.orig_file_size; k++) {
                            uint32_t value_to_store = (1 << poz_last_bit) * ((orig[j] >> k) & 1);
                            last_char |= value_to_store;
                            poz_last_bit++;
                            if (poz_last_bit == 8) {
                                file_to << last_char;
                                already_taken++;
                                last_char = 0;
                                poz_last_bit = 0;
                            }
                        }
                    }

                    uint32_t cnt_last =
                        (cur_file.before_hamming_block_size % 8) + (cur_file.before_hamming_block_size % 8 == 0) * 8;
                    for (uint32_t j = 0; j < cnt_last && already_taken < cur_file.orig_file_size; j++) {
                        uint32_t value_to_store = (1 << poz_last_bit) * ((orig.back() >> j) & 1);
                        last_char |= value_to_store;
                        poz_last_bit++;
                        if (poz_last_bit == 8) {
                            file_to << last_char;
                            already_taken++;
                            last_char = 0;
                            poz_last_bit = 0;
                        }
                    }

                    cur_poz = 0;
                    SetZeros(bits, after_hamming_block_size);

                    break;
                }
            }
        }

        file_to.close();
        file.get(cur);
    }

    file.close();
    temp_file.close();

    char* first_file_name = &archive.archive_name[0];
    char* second_file_name = &temp_file_name[0];

    if (!archive.is_all_files) { // was created temp_file
        std::remove(first_file_name);
        if (!has_one_file) { // nothing was kept in archive
            std::remove(second_file_name);
        } else { //something was kept in archive
            std::rename(second_file_name, first_file_name);
        }
    } else { // nothing was kept in archive
        std::remove(first_file_name);
    }
}

void DeleteFile(ArchiveInfo& archive, char* file_name_delete) {
    std::string temp_file_name = "$temp$.haf";
    std::ofstream file_temp;
    file_temp.open(temp_file_name);

    std::ifstream file;
    file.open(archive.archive_name);
    if (!file.is_open()) {
        std::string error = "Can't open archive";
        ThrowBug(error);
    }

    char cur = 0;
    uint32_t cnt_written = 0;
    bool was_deleted = false;
    while (!file.eof()) {
        HeaderInfo cur_file;
        ReadHeader(file, cur_file);

        if (file.eof()) {
            break;
        }

        uint32_t hamming_file_size = CalculateRealFileSize(cur_file.orig_file_size, cur_file.before_hamming_block_size);

        char* cur_file_name = &cur_file.file_name[0];
        if (strcmp(cur_file_name, file_name_delete) == 0) { // found file we need
            file.seekg(hamming_file_size + 1, std::ios_base::cur);
            was_deleted = true;
            continue;
        }

        cnt_written++;
        file_temp << cur_file.file_name << " " << cur_file.orig_file_size << " " << cur_file.before_hamming_block_size
                  << '\n';
        for (uint32_t i = 0; i < hamming_file_size; i++) {
            file.get(cur);
            file_temp << cur;
        }
        file.get(cur);
        file_temp << cur;
    }
    file.close();
    file_temp.close();

    char* first_file_name = &temp_file_name[0];
    char* second_file_name = &archive.archive_name[0];

    if (cnt_written == 0 && was_deleted) {
        std::remove(first_file_name);
        std::remove(second_file_name);
    } else if (!was_deleted) {
        std::remove(first_file_name);
    } else {
        std::remove(second_file_name);
        std::rename(first_file_name, second_file_name);
    }
}

void MergeArchives(ArchiveInfo& archive) {
    std::ofstream result_file;
    result_file.open(archive.archive_name);

    std::ifstream first_arch;
    first_arch.open(archive.first_archive_name);
    if (!first_arch.is_open()) {
        std::string error = "Can't open ";
        for (uint32_t i = 0; i < strlen(archive.first_archive_name); i++) {
            error += archive.first_archive_name[i];
        }
        ThrowBug(error);
    }

    std::ifstream second_arch;
    second_arch.open(archive.second_archive_name);
    if (!second_arch.is_open()) {
        std::string error = "Can't open ";
        for (uint32_t i = 0; i < strlen(archive.second_archive_name); i++) {
            error += archive.second_archive_name[i];
        }
        ThrowBug(error);
    }

    char cur;
    while (!first_arch.eof()) {
        first_arch.get(cur);
        if (!first_arch.eof()) {
            result_file << cur;
        }
    }

    while (!second_arch.eof()) {
        second_arch.get(cur);
        if (!second_arch.eof()) {
            result_file << cur;
        }
    }
    result_file.close();
    first_arch.close();
    second_arch.close();

    std::remove(archive.first_archive_name);
    std::remove(archive.second_archive_name);
}
