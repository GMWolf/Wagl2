//
// Created by felix on 5/11/2019.
//

#include "FrameBuffer.h"

wagl::gl::FrameBuffer::FrameBuffer() {
    glCreateFramebuffers(1, &id);
}

wagl::gl::FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &id);
}

void wagl::gl::FrameBuffer::texture(GLenum attachment, wagl::gl::Texture &texture, GLint level) {
    glNamedFramebufferTexture(id, attachment, texture.id, level);
}

void wagl::gl::FrameBuffer::drawBuffers(std::initializer_list<GLenum> attachements) {
    glNamedFramebufferDrawBuffers(id, attachements.size(), attachements.begin());
}

void wagl::gl::FrameBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void wagl::gl::FrameBuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}
