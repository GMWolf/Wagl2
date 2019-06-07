//
// Created by felix on 5/11/2019.
//

#ifndef WAGL2_PINGPONGBUFFER_H
#define WAGL2_PINGPONGBUFFER_H

#include "gl/FrameBuffer.h"
#include <optional>

namespace wagl {
    class PingPongBuffer {

        static const size_t FRAME_COUNT = 2;

        gl::FrameBuffer frameBuffers[FRAME_COUNT];
        gl::Texture textures[FRAME_COUNT];
        gl::TextureHandle handles[FRAME_COUNT];

        size_t index = 0;
        std::optional<gl::TextureHandle> source;

    public:
        PingPongBuffer(size_t width, size_t height, GLenum format);

        void makeResident();

        void makeNonResident();

        void setSource(const gl::TextureHandle& handle);

        void rotate();

        gl::TextureHandle handle();

        void bind();

    };
}


#endif //WAGL2_PINGPONGBUFFER_H
