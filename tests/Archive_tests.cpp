#include <gtest/gtest.h>
#include <filesystem>
#include "write.h"

bool IsEqualFiles(std::string& f, std::string& s) {
    std::ifstream first;
    first.open(f, std::ios::binary);

    std::ifstream second;
    second.open(s, std::ios::binary);

    char cur_first;
    char cur_second;
    while (!first.eof() && !second.eof()) {
        first >> cur_first;
        second >> cur_second;

        if (!first.eof() && !second.eof()) {
            if (cur_first != cur_second) {
                return false;
            }
        } else if (first.eof() || second.eof()) {
            break;
        }
    }
    return (first.eof() == second.eof());
}

void MakeToUse(std::string& path, std::vector<std::string>& files) {
    for (size_t i = 0; i < files.size(); i++) {
        std::string file_from = "../test_orig/" + files[i];
        std::string file_to = path + files[i];

        std::filesystem::copy(file_from, file_to);
    }
}

ArchiveInfo MakeArchive(std::vector<std::string>& files, std::string& path, std::string name) {
    ArchiveInfo archive;
    archive.archive_name = path + name;
    archive.cnt_add_bits = CalculateAddBits(archive.cnt_bits_in_block);

    for (size_t i = 0; i < files.size(); i++) {
        std::string file_name = path + files[i];
        files[i] = file_name;
        archive.files.push_back(&files[i][0]);
    }
    CreateArchive(archive);
    return archive;
}

TEST(ArchiveTests, CodeTextFile) {
    std::vector<std::string> files = {"d.txt"};
    std::filesystem::create_directories("../test_1");

    std::string path = "../test_1/";
    std::string orig = "../test_orig/";
    MakeToUse(path, files);

    ArchiveInfo archive = MakeArchive(files, path, "test.haf");

    ExtractArchive(archive);

    bool is_correct = true;
    for (size_t i = 0; i < files.size(); i++) {
        std::string origin_file = orig + files[i].substr(10, 5);
        is_correct &= IsEqualFiles(files[i], origin_file);
    }

    std::filesystem::remove_all("../test_1");
    ASSERT_TRUE(is_correct);
}

TEST(ArchiveTests, CodeBmpFile) {
    std::vector<std::string> files = {"b.bmp"};
    std::filesystem::create_directories("../test_2");

    std::string path = "../test_2/";
    std::string orig = "../test_orig/";
    MakeToUse(path, files);

    ArchiveInfo archive = MakeArchive(files, path, "test.haf");
    ExtractArchive(archive);

    bool is_correct = true;
    for (size_t i = 0; i < files.size(); i++) {
        std::string origin_file = orig + files[i].substr(10, 5);
        is_correct &= IsEqualFiles(files[i], origin_file);
    }

    std::filesystem::remove_all("../test_2");
    ASSERT_TRUE(is_correct);
}

TEST(ArchiveTests, CodeMp3File) {
    std::vector<std::string> files = {"c.mp3"};
    std::filesystem::create_directories("../test_3");

    std::string path = "../test_3/";
    std::string orig = "../test_orig/";
    MakeToUse(path, files);

    ArchiveInfo archive = MakeArchive(files, path, "test.haf");
    ExtractArchive(archive);

    bool is_correct = true;
    for (size_t i = 0; i < files.size(); i++) {
        std::string origin_file = orig + files[i].substr(10, 5);
        is_correct &= IsEqualFiles(files[i], origin_file);
    }

    std::filesystem::remove_all("../test_3");
    ASSERT_TRUE(is_correct);
}

TEST(ArchiveTests, CheckList) {
    std::vector<std::string> files = {"b.bmp", "c.mp3", "d.txt"};
    std::filesystem::create_directories("../test_4");

    std::string path = "../test_4/";
    MakeToUse(path, files);

    ArchiveInfo archive = MakeArchive(files, path, "test.haf");
    std::vector<std::string> file_list = GetArchiveFiles(archive);

    bool is_correct = (file_list == files);
    std::filesystem::remove_all("../test_4");
    ASSERT_TRUE(is_correct);
}

TEST(ArchiveTests, DeleteAndCheck) {
    std::vector<std::string> files = {"c.mp3", "d.txt"};
    std::filesystem::create_directories("../test_5");

    std::string path = "../test_5/";
    MakeToUse(path, files);

    ArchiveInfo archive = MakeArchive(files, path, "test.haf");
    DeleteFile(archive, &files.back()[0]);
    files.pop_back();

    std::vector<std::string> file_list = GetArchiveFiles(archive);

    bool is_correct = (file_list == files);
    std::filesystem::remove_all("../test_5");
    ASSERT_TRUE(is_correct);
}

TEST(ArchiveTests, ArchiveOfArchives) {
    std::string path = "../test_6/";
    std::filesystem::create_directories("../test_6");

    std::vector<std::string> files_1 = {"d.txt"};
    MakeToUse(path, files_1);
    ArchiveInfo archive_1 = MakeArchive(files_1, path, "test1.haf");


    std::vector<std::string> files_2 = {"b.bmp"};
    MakeToUse(path, files_2);
    ArchiveInfo archive_2 = MakeArchive(files_2, path, "test2.haf");


    std::vector<std::string> files_3 = {"test1.haf", "test2.haf"};
    ArchiveInfo archive_3 = MakeArchive(files_3, path, "test3.haf");

    ExtractArchive(archive_3);
    ExtractArchive(archive_2);
    ExtractArchive(archive_1);

    std::string orig_file_1 = "../test_orig/d.txt";
    bool is_correct = IsEqualFiles(files_1[0], orig_file_1);

    std::string orig_file_2 = "../test_orig/b.bmp";
    is_correct &= IsEqualFiles(files_2[0], orig_file_2);

    std::filesystem::remove_all("../test_6");
    ASSERT_TRUE(is_correct);
}

TEST(ArchiveTests, CodeMp4File) {
    std::vector<std::string> files = {"a.mp4"};
    std::filesystem::create_directories("../test_8");

    std::string path = "../test_8/";
    std::string orig = "../test_orig/";
    MakeToUse(path, files);

    ArchiveInfo archive = MakeArchive(files, path, "test.haf");
    ExtractArchive(archive);

    bool is_correct = true;
    for (size_t i = 0; i < files.size(); i++) {
        std::string origin_file = orig + files[i].substr(10, 5);
        is_correct &= IsEqualFiles(files[i], origin_file);
    }

    std::filesystem::remove_all("../test_8");
    ASSERT_TRUE(is_correct);
}

TEST(ArchiveTests, CodeCoupleFiles) {
    std::vector<std::string> files = {"a.mp4", "b.bmp", "c.mp3", "d.txt"};
    std::filesystem::create_directories("../test_9");

    std::string path = "../test_9/";
    std::string orig = "../test_orig/";
    MakeToUse(path, files);

    ArchiveInfo archive = MakeArchive(files, path, "test.haf");
    ExtractArchive(archive);

    bool is_correct = true;
    for (size_t i = 0; i < files.size(); i++) {
        std::string origin_file = orig + files[i].substr(10, 5);
        is_correct &= IsEqualFiles(files[i], origin_file);
    }

    std::filesystem::remove_all("../test_9");
    ASSERT_TRUE(is_correct);
}