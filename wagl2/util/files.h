//
// Created by felix on 4/3/2019.
//

#ifndef WAGL2_FILES_H
#define WAGL2_FILES_H

#include <vector>
#include <string>

namespace wagl {
    std::string loadFile(const std::string& path);
    std::vector<char> loadFileBinary(const std::string& path);

    std::string loadFileGLSL(const std::string& path, const std::string& includeDir);
}

#endif //WAGL2_FILES_H
