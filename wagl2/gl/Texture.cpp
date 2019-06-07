//
// Created by felix on 5/5/2019.
//

#include "Texture.h"

wagl::gl::TextureHandle::TextureHandle() : id(0){}

wagl::gl::TextureHandle::TextureHandle(GLuint64 h) : id(h) {}

void wagl::gl::TextureHandle::makeResident() {
    glMakeTextureHandleResidentARB(id);
}

void wagl::gl::TextureHandle::makeNonResident() {
    glMakeTextureHandleNonResidentARB(id);
}

wagl::gl::ImageHandle::ImageHandle() : id(0) {}

wagl::gl::ImageHandle::ImageHandle(GLuint64 h) : id(h) {}

void wagl::gl::ImageHandle::makeResident(GLenum access) {
    glMakeImageHandleResidentARB(id, access);
}

void wagl::gl::ImageHandle::makeNonResident() {
    glMakeImageHandleNonResidentARB(id);
}

wagl::gl::Texture::Texture(GLenum target) :
width(0), height(0), depth(0), levels(0)
{
    glCreateTextures(target, 1, &id);
}

wagl::gl::Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void wagl::gl::Texture::storage2D_internal(GLsizei _width, GLsizei _height, GLsizei _levels, GLenum internalFormat) {
    levels = _levels;
    width = _width;
    height = _height;
    depth = 1;
    glTextureStorage2D(id, levels, internalFormat, _width, _height);
}


void wagl::gl::Texture::storage2D(GLsizei _width, GLsizei _height, GLsizei _levels, GLenum internalFormat) {
    storage2D_internal(_width, _height, _levels, internalFormat);
}

void wagl::gl::Texture::storage2DSparse(GLsizei _width, GLsizei _height, GLsizei _levels, GLenum internalFormat) {
    parameter(GL_TEXTURE_SPARSE_ARB, true);
    storage2D_internal(_width, _height, _levels, internalFormat);
}

void wagl::gl::Texture::storage3D_internal(GLsizei _width, GLsizei _height, GLsizei _depth, GLsizei _levels, GLenum internalFormat) {
    levels = _levels;
    width = _width;
    height = _height;
    depth = _depth;
    glTextureStorage3D(id, levels, internalFormat, width, height, depth);
}



void wagl::gl::Texture::storage3D(GLsizei _width, GLsizei _height, GLsizei _depth, GLsizei _levels, GLenum internalFormat) {
    storage3D_internal(_width, _height, _depth, _levels, internalFormat);
}

void wagl::gl::Texture::storage3DSparse(GLsizei _width, GLsizei _height, GLsizei _depth, GLsizei _levels, GLenum internalFormat) {
    parameter(GL_TEXTURE_SPARSE_ARB, true);
    storage3D_internal(_width, _height, _depth, _levels, internalFormat);
}

void wagl::gl::Texture::subimage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
        GLenum format, GLenum type, const void *data) {
    glTextureSubImage2D(id, level, xoffset, yoffset, width, height, format, type, data);
}

void
wagl::gl::Texture::submimage3D(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height,
                               GLsizei depth, GLenum format, GLenum type, const void *data) {
    glTextureSubImage3D(id, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data);
}



void wagl::gl::Texture::invalidate(GLint level) {
    glInvalidateTexImage(id, level);
}

void wagl::gl::Texture::parameter(GLenum param, GLint value) {
    glTextureParameteri(id, param, value);
}

void wagl::gl::Texture::parameter(GLenum param, const GLint *values) {
    glTextureParameteriv(id, param, values);
}

void wagl::gl::Texture::parameter(GLenum param, GLfloat value) {
    glTextureParameterf(id, param, value);
}

void wagl::gl::Texture::parameter(GLenum param, GLfloat *values) {
    glTextureParameterfv(id, param, values);
}

int wagl::gl::Texture::getParameteri(GLenum param) {
    GLint result;
    glGetTextureParameteriv(id, param, &result);
    return result;
}

void wagl::gl::Texture::invalidate() {
    for(GLint level = 0; level < levels; level++) {
        invalidate(level);
    }
}

wagl::gl::TextureHandle wagl::gl::Texture::getHandle() {
    return TextureHandle(glGetTextureHandleARB(id));
}

wagl::gl::TextureHandle wagl::gl::Texture::getHandle(const wagl::gl::Sampler &sampler) {
    return TextureHandle(glGetTextureSamplerHandleARB(id, sampler.id));
}

wagl::gl::ImageHandle wagl::gl::Texture::getImageHandle(GLint level, bool layered, GLint layer, GLenum format) {
    return ImageHandle(glGetImageHandleARB(id, level, layered, layer, format));
}


void wagl::gl::Texture::pageCommitment(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
                                       GLsizei height, GLsizei depth, bool commit) {
    glTexturePageCommitmentEXT(id, level, xoffset, yoffset, zoffset, width, height, depth, commit);
}


wagl::gl::Sampler::Sampler() {
    glCreateSamplers(1, &id);
}

wagl::gl::Sampler::Sampler(std::initializer_list<SamplerParamDescriptor> params) : Sampler() {
    for(auto& param : params) {
        std::visit([&](auto value) {
            parameter(param.name, value);
        }, param.value);
    }
}

wagl::gl::Sampler::~Sampler() {
    glDeleteSamplers(1, &id);
}

void wagl::gl::Sampler::parameter(GLenum name, GLint param) {
    glSamplerParameteri(id, name, param);
}

void wagl::gl::Sampler::parameter(GLenum name, GLfloat param) {
    glSamplerParameterf(id, name, param);
}





