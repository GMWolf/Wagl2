//
// Created by felix on 07/06/2019.
//

#ifndef WAGL2_VIRTUALTEXTURE_H
#define WAGL2_VIRTUALTEXTURE_H

#include "gl/Texture.h"
#include <glm/vec2.hpp>
#include <vector>

namespace wagl {

    class VirtualTextureMap {
    public:
        VirtualTextureMap(glm::uvec2 size, size_t levels) : size(size), levels(levels){
        }

    private:
        glm::uvec2 size;
        size_t levels;

        std::vector<gl::Texture> textures;
    };

}

#endif //WAGL2_VIRTUALTEXTURE_H
