#include "FileSystem.hpp"


void FileSystem::setWorkingDirectory(const std::string &programWorkingDirectory) {
    try {
        std::filesystem::current_path(programWorkingDirectory);
    } catch (const std::filesystem::filesystem_error &e) {
        AT_FATAL("Error changing directory: {0}", e.what());
    }
}

std::string FileSystem::fileToString(const std::string &filePath) {
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        AT_ERROR("Could not open file: {0}", filePath);
    }

    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}
