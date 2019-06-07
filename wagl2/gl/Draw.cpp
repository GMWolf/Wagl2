//
// Created by felix on 15/01/2019.
//

#include "Draw.h"

void wagl::gl::DrawArrays(const wagl::gl::VertexArray &va, GLenum mode, GLint first, GLint count, const wagl::gl::Shader &shader) {

    shader.use();
    glBindVertexArray(va.id);
    glDrawArrays(mode, first, count);

}