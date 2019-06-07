//
// Created by felix on 4/3/2019.
//

#include "files.h"
#include <fstream>
#include <ios>
#include <stb_include.h>


std::vector<char> loadFileInternal(std::ifstream& file) {
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

std::string wagl::loadFile(const std::string &path) {

    std::ifstream file(path);

    if(!file) {
        throw std::runtime_error("failed to open file");
    }

    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

}

std::vector<char> wagl::loadFileBinary(const std::string &path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if(!file) {
        throw std::runtime_error("Failed to open file");
    }

    return loadFileInternal(file);
}

std::string wagl::loadFileGLSL(const std::string &path, const std::string& includeDir) {

    char error[256];
    char* result = stb_include_file(const_cast<char*>(path.c_str()), nullptr, const_cast<char*>(includeDir.c_str()), error);

    if (!result) {
        throw std::runtime_error(error);
    }

    auto res = std::string(result);
    free(result);
    return res;
}
