//
// Created by felix on 07/06/2019.
//

#include "VirtualTexture.h"

wagl::VirtualTexture::VirtualTexture(glm::uvec2 size, size_t levels, GLenum format) :
        size(size),
        texture(GL_TEXTURE_2D)
{
    texture.storage2DSparse(size.x, size.y, levels, format);
}
