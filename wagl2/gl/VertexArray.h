//
// Created by felix on 15/01/2019.
//

#ifndef WAGL2_VERTEXARRAY_H
#define WAGL2_VERTEXARRAY_H

#include "Object.h"
#include "Buffer.h"

namespace wagl::gl {

        class VertexArray : public Object {
        public:
            VertexArray();

            ~VertexArray();

            VertexArray(VertexArray &&) noexcept = default;

            VertexArray &operator=(VertexArray &&) noexcept = default;

            void attribFormat(GLuint attribIndex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);

            void vertexBuffer(GLuint bindIndex, const BaseBuffer &buffer, GLintptr offset, GLsizei stride);

            void vertexBuffer(GLuint bindIndex, BufferSlice slice, GLsizei stride);

            void attribBinding(GLuint attribindex, GLuint bindIndex);

            void enableAttrib(GLuint index);

            void disableAttrib(GLuint index);

            void elementBuffer(const BaseBuffer &buffer);

            void bind() const;
        };

    }

#endif //WAGL2_VERTEXARRAY_H
