//
// Created by felix on 15/01/2019.
//

#ifndef WAGL2_DRAW_H
#define WAGL2_DRAW_H


#include "VertexArray.h"
#include "Shader.h"

namespace wagl {
    namespace gl {

        void DrawArrays(const VertexArray &va, GLenum mode, GLint first, GLint count, const Shader &shader);

    }

}


#endif //WAGL2_DRAW_H
