//
// Created by lutyk on 12/15/24.
//
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <cstdlib>

namespace fs = std::filesystem;

std::string shortenName(const std::string &name, std::unordered_map<std::string, int> &conflictMap) {
    std::string base, ext;
    size_t dotPos = name.find_last_of('.');

    if (dotPos == std::string::npos) {
        base = name;
        ext = "";
    } else {
        base = name.substr(0, dotPos);
        ext = name.substr(dotPos + 1);
    }

    if (base.length() > 8) base = base.substr(0, 8);
    if (ext.length() > 3) ext = ext.substr(0, 3);

    std::string shortName = base + (ext.empty() ? "" : "." + ext);

    if (conflictMap.find(shortName) != conflictMap.end()) {
        int counter = ++conflictMap[shortName];
        std::ostringstream oss;
        oss << base.substr(0, std::min(size_t(6), base.length())) << "~" << counter
            << (ext.empty() ? "" : "." + ext);
        shortName = oss.str();
    } else {
        conflictMap[shortName] = 0;
    }

    return shortName;
}

void prepareFiles(const fs::path &source, const fs::path &target, std::unordered_map<std::string, int> &conflictMap) {
    for (const auto &entry : fs::directory_iterator(source)) {
        fs::path targetPath = target / shortenName(entry.path().filename().string(), conflictMap);

        if (entry.is_directory()) {
            fs::create_directory(targetPath);
            prepareFiles(entry.path(), targetPath, conflictMap);
        } else if (entry.is_regular_file()) {
            fs::copy(entry.path(), targetPath);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <directory> <output ISO file>\n";
        return 1;
    }

    fs::path sourceDir = argv[1];
    fs::path isoFile = argv[2];

    if (!fs::exists(sourceDir) || !fs::is_directory(sourceDir)) {
        std::cerr << "Error: Provided path is not a directory or does not exist.\n";
        return 1;
    }

    fs::path tempDir = fs::temp_directory_path() / ("iso_build_" + std::to_string(std::time(nullptr)));
    try {
        fs::create_directory(tempDir);
    } catch (const std::exception &e) {
        std::cerr << "Error: Unable to create temporary directory: " << e.what() << "\n";
        return 1;
    }

    std::unordered_map<std::string, int> conflictMap;
    prepareFiles(sourceDir, tempDir, conflictMap);

    std::ostringstream cmd;
    cmd << "mkisofs -o \"" << isoFile.string() << "\" -J -R \"" << tempDir.string() << "\"";
    int result = system(cmd.str().c_str());

    fs::remove_all(tempDir);

    if (result == 0) {
        std::cout << "ISO image successfully created: " << isoFile.string() << "\n";
    } else {
        std::cerr << "Error: Failed to create ISO image. Make sure mkisofs is installed.\n";
    }

    return result;
}
