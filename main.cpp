#include "parser/parser.h"
#include "write/write.h"

int main(int argc, char** argv) {
    ArchiveInfo archive;
    ParseArguments(argc, argv, archive);

    if (archive.is_create) {
        CreateArchive(archive);
    }
    if (archive.is_list) {
        std::vector<std::string> files = GetArchiveFiles(archive);
        for (size_t i = 0; i < files.size(); i++) {
            std::cout << files[i] << '\n';
        }
    }
    if (archive.is_extract) {
        ExtractArchive(archive);
    }
    if (archive.is_append) {
        AddFileToArchive(archive, archive.files[0]);
    }
    if (archive.is_delete) {
        DeleteFile(archive, archive.files[0]);
    }
    if (archive.is_concatenate) {
        MergeArchives(archive);
    }

    return 0;
}
