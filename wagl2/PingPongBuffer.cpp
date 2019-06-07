//
// Created by felix on 5/11/2019.
//

#include "PingPongBuffer.h"

wagl::PingPongBuffer::PingPongBuffer(size_t width, size_t height, GLenum format)
        : textures{gl::Texture(GL_TEXTURE_2D), gl::Texture(GL_TEXTURE_2D)} {
    for (size_t i = 0; i < FRAME_COUNT; i++) {
        textures[i].storage2D(width, height, 1, format);
        textures[i].parameter( GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        textures[i].parameter( GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        handles[i] = textures[i].getHandle();
        frameBuffers[i].texture(GL_COLOR_ATTACHMENT0, textures[i], 0);
        frameBuffers->drawBuffers({GL_COLOR_ATTACHMENT0});
    }
}

void wagl::PingPongBuffer::makeResident() {
    for(auto & handle : handles) {
        handle.makeResident();
    }
}

void wagl::PingPongBuffer::makeNonResident() {
    for(auto& handle : handles) {
        handle.makeNonResident();
    }
}

void wagl::PingPongBuffer::rotate() {
    source = {};
    index ++;
    if (index == FRAME_COUNT)
        index = 0;
}

wagl::gl::TextureHandle wagl::PingPongBuffer::handle() {
    if (source) return *source;
    return handles[(index == 0) ? (FRAME_COUNT - 1) : (index - 1)];
}

void wagl::PingPongBuffer::bind() {
    frameBuffers[index].bind();
}

void wagl::PingPongBuffer::setSource(const wagl::gl::TextureHandle &handle) {
    source = handle;
}
