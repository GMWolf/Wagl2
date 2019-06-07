//
// Created by felix on 14/01/2019.
//

#ifndef WAGL2_BUFFER_H
#define WAGL2_BUFFER_H

#include <glad/glad.h>
#include <vector>
#include "Object.h"
#include "../util/Utils.h"

namespace wagl::gl {

        class BaseBuffer : public Object {
        public:
            BaseBuffer() = default;

            ~BaseBuffer() = default;

            BaseBuffer(BaseBuffer &&) = default;

            BaseBuffer &operator=(BaseBuffer &&) = default;

            void bindRange(GLenum target, GLuint index, GLintptr offset, GLsizeiptr size) const;

            void bind(GLenum target) const;
        };

        class BufferSlice {
        public:
            BaseBuffer &base;

            BufferSlice(BaseBuffer &base, GLintptr start, GLsizeiptr size) :
                    base(base), start(start), size(size) {
            }

            BufferSlice(BaseBuffer &base) : base(base), start(0), size(0) {
            }

            const GLintptr start;
            const GLsizeiptr size;
        };

        class Buffer : public BaseBuffer {
        public:
            GLsizeiptr size;

            Buffer();

            Buffer(GLsizeiptr size, const void *data, GLbitfield flags);

            Buffer(const util::block& block, GLbitfield flags);;

            Buffer(Buffer &&) noexcept = default;
            Buffer &operator=(Buffer &&) noexcept = default;

            template<class T>
            explicit Buffer(const std::vector<T> &data, GLbitfield flags = 0);

            template<class T>
            Buffer(std::initializer_list<T> data);

            ~Buffer();

            void storage(GLsizeiptr size, const void* data, GLbitfield flags);

            void bufferSubData(GLintptr offset, GLsizeiptr size, const GLvoid *data);

            void bufferData(GLsizeiptr size, const GLvoid *data);

            template<class T>
            void bufferData(const std::vector<T> &data);

            void clearData(GLenum internalformat, GLenum format, GLenum type, const void *data);

            void glCopySubData(const Buffer &source, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);

            void *mapRange(GLintptr offset, GLsizeiptr length, GLbitfield flags);

            bool unmap();

            void flushMappedRange(GLintptr offset, GLsizeiptr length);

            void invalidateData();

            void invalidateData(GLintptr offset, GLsizeiptr length);

            BufferSlice slice(GLintptr offset, GLsizeiptr length);
        };
    }

template<class T>
void wagl::gl::Buffer::bufferData(const std::vector<T> &data) {
    bufferData(sizeof(T) + data.size(), data.data());
}

template<class T>
wagl::gl::Buffer::Buffer(const std::vector<T> &data, GLbitfield flags) : Buffer(sizeof(T) * data.size(), data.data(), flags) {
}

template<class T>
wagl::gl::Buffer::Buffer(std::initializer_list<T> data) : Buffer(std::vector<T>(data)){}


#endif //WAGL2_BUFFER_H
