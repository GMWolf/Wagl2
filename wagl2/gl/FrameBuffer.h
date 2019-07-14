//
// Created by felix on 5/11/2019.
//

#ifndef WAGL2_FRAMEBUFFER_H
#define WAGL2_FRAMEBUFFER_H

#include "Object.h"
#include "Texture.h"
#include <initializer_list>

namespace wagl::gl {

    class FrameBuffer : public Object {
    public:
        FrameBuffer();

        ~FrameBuffer();

        FrameBuffer(FrameBuffer&&) = default;
        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer& operator=(FrameBuffer&&) = default;
        FrameBuffer& operator=(const FrameBuffer&) = delete;

        void texture(GLenum attachment, Texture& texture, GLint level);

        void drawBuffers(std::initializer_list<GLenum> attachements);

        void bind();

        void read(GLenum mode);

        static void unbind();
    };

}

#endif //WAGL2_FRAMEBUFFER_H
