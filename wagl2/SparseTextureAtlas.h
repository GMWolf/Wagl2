//
// Created by felix on 06/06/2019.
//

#ifndef WAGL2_SPARSETEXTUREATLAS_H
#define WAGL2_SPARSETEXTUREATLAS_H

#include "gl/Texture.h"
#include <glm/glm.hpp>
#include <vector>
#include <Geom.h>
#include <ctype.h>
#include "util/Matrix.h"

namespace wagl {

    class SparseTextureAtlas {
    public:
        SparseTextureAtlas(glm::uvec2 virtualSize, size_t levels, GLenum format);

        gl::TextureHandle gethandle(const gl::Sampler& sampler);

        urect alloc(glm::uvec2 size);

        gl::Texture texture;
    private:
        glm::uvec2 virtualSize;
        GLsizei levels;
        glm::uvec2 pageSize;
        glm::uvec2 maxOrder;
        glm::uvec2 pageOrder;

        //matrix of freelists
        using freelist_t = wagl::Matrix<std::vector<urect>>;
        freelist_t freelists;

        urect allocOrder(glm::uvec2 order);

    };

}

#endif //WAGL2_SPARSETEXTUREATLAS_H
