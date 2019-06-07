//
// Created by felix on 14/01/2019.
//

#include <stdexcept>
#include "Buffer.h"

wagl::gl::Buffer::Buffer() : size(0){
    glCreateBuffers(1, &id);
}

wagl::gl::Buffer::Buffer(GLsizeiptr size, const void *data, GLbitfield flags) : size(size) {
    glCreateBuffers(1, &id);
    glNamedBufferStorage(id, size, data, flags);
}

void wagl::gl::Buffer::storage(GLsizeiptr _size, const void *data, GLbitfield flags) {
    size = _size;
    glNamedBufferStorage(id, size, data, flags);
}


void wagl::gl::Buffer::bufferSubData(GLintptr offset, GLsizeiptr size, const GLvoid *data) {
    glNamedBufferSubData(id, offset, size, data);
}

void wagl::gl::Buffer::clearData(GLenum internalformat, GLenum format, GLenum type, const void *data) {
    glClearNamedBufferData(id, internalformat, format, type, data);
}

void wagl::gl::Buffer::glCopySubData(const wagl::gl::Buffer &source, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
    glCopyNamedBufferSubData(source.id, id, readOffset, writeOffset, size);
}

wagl::gl::Buffer::~Buffer() {
    glDeleteBuffers(1, &id);
}

void *wagl::gl::Buffer::mapRange(GLintptr offset, GLsizeiptr length, GLbitfield flags) {
    return glMapNamedBufferRange(id, offset, length, flags);
}

void wagl::gl::Buffer::bufferData(GLsizeiptr size, const GLvoid *data) {
    glNamedBufferSubData(id, 0, size, data);
}

void wagl::gl::Buffer::invalidateData() {
    glInvalidateBufferData(id);
}

void wagl::gl::Buffer::invalidateData(GLintptr offset, GLsizeiptr length) {
    glInvalidateBufferSubData(id, offset, length);
}

bool wagl::gl::Buffer::unmap() {
    return glUnmapNamedBuffer(id);
}

wagl::gl::BufferSlice wagl::gl::Buffer::slice(GLintptr offset, GLsizeiptr length) {
    return BufferSlice(*this, offset, length);
}

wagl::gl::Buffer::Buffer(const wagl::util::block &block, GLbitfield flags) : Buffer(block.size, block.get(), flags) {}

void wagl::gl::Buffer::flushMappedRange(GLintptr offset, GLsizeiptr length) {
    glFlushMappedNamedBufferRange(id, offset, length);
}

void wagl::gl::BaseBuffer::bindRange(GLenum target, GLuint index, GLintptr offset, GLsizeiptr size) const {
    glBindBufferRange(target, index, id, offset, size);
}

void wagl::gl::BaseBuffer::bind(GLenum target) const {
    glBindBuffer(target, id);
}
