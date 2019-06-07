//
// Created by felix on 15/01/2019.
//

#include "VertexArray.h"

wagl::gl::VertexArray::VertexArray() {
    glCreateVertexArrays(1, &id);
}

wagl::gl::VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &id);
}

void wagl::gl::VertexArray::enableAttrib(GLuint index) {
    glEnableVertexArrayAttrib(id, index);
}

void wagl::gl::VertexArray::attribFormat(GLuint attribIndex, GLint size, GLenum type, GLboolean normalized,
                                     GLuint relativeoffset) {
    glVertexArrayAttribFormat(id, attribIndex, size, type, normalized, relativeoffset);
}

void wagl::gl::VertexArray::vertexBuffer(GLuint bindIndex, const wagl::gl::BaseBuffer &buffer, GLintptr offset, GLsizei stride) {
    glVertexArrayVertexBuffer(id, bindIndex, buffer.id, offset, stride);
}

void wagl::gl::VertexArray::vertexBuffer(GLuint bindIndex, const wagl::gl::BufferSlice slice, GLsizei stride) {
    vertexBuffer(bindIndex, slice.base, slice.start, stride);
}

void wagl::gl::VertexArray::attribBinding(GLuint attribindex, GLuint bindIndex) {
    glVertexArrayAttribBinding(id, attribindex, bindIndex);
}

void wagl::gl::VertexArray::disableAttrib(GLuint index) {
    glDisableVertexArrayAttrib(id, index);
}

void wagl::gl::VertexArray::elementBuffer(const wagl::gl::BaseBuffer &buffer) {
    glVertexArrayElementBuffer(id, buffer.id);
}

void wagl::gl::VertexArray::bind() const {
    glBindVertexArray(id);
}



