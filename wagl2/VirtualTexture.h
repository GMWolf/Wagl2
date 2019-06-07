//
// Created by felix on 07/06/2019.
//

#ifndef WAGL2_VIRTUALTEXTURE_H
#define WAGL2_VIRTUALTEXTURE_H

#include "gl/Texture.h"
#include <glm/vec2.hpp>

namespace wagl {

    class VirtualTexture {
    public:
        VirtualTexture(glm::uvec2 size, size_t levels, GLenum format);

    private:
        glm::uvec2 size;
        gl::Texture texture;
    };

}

#endif //WAGL2_VIRTUALTEXTURE_H
