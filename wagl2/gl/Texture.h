//
// Created by felix on 5/5/2019.
//

#ifndef WAGL2_TEXTURE_H
#define WAGL2_TEXTURE_H

#include "Object.h"
#include <initializer_list>
#include <variant>

namespace wagl::gl {

    class Texture;

    class TextureHandle {
        friend class wagl::gl::Texture;

    public:
        TextureHandle();
        TextureHandle(TextureHandle&&) noexcept = default;
        TextureHandle(const TextureHandle&) = default;
        TextureHandle& operator=(TextureHandle&&) noexcept = default;
        TextureHandle& operator=(const TextureHandle&) = default;
        void makeResident();

        void makeNonResident();

    private:
        GLuint64 id;

        explicit TextureHandle(GLuint64 h);
    };

    class ImageHandle {
        friend class wagl::gl::Texture;
    public:
        ImageHandle();
        ImageHandle(ImageHandle&&) noexcept = default;
        ImageHandle(const ImageHandle&) = default;
        ImageHandle& operator=(ImageHandle&&) noexcept = default;
        ImageHandle& operator=(const ImageHandle&) = default;
        void makeResident(GLenum access);
        void makeNonResident();
    private:
        GLuint64 id;
        explicit ImageHandle(GLuint64 h);
    };

    class Sampler : public Object {

        struct SamplerParamDescriptor {
            GLenum name;
            std::variant<GLint, GLfloat> value;
        };

    public:
        Sampler();

        Sampler(std::initializer_list<SamplerParamDescriptor> params);

        ~Sampler();

        Sampler(Sampler&&) = default;
        Sampler(const Sampler&) = delete;
        Sampler& operator=(Sampler&&) = default;
        Sampler& operator=(const Sampler&) = delete;

        void parameter(GLenum name, GLint param);

        void parameter(GLenum name, GLfloat param);
    };

    class Texture : public Object {
    public:
        Texture(Texture &&) = default;

        Texture(const Texture &) = delete;

        Texture &operator=(Texture &&) = default;

        Texture &operator=(const Texture &) = delete;

        void storage2D(GLsizei width, GLsizei height, GLsizei levels, GLenum internalFormat);

        void storage2DSparse(GLsizei width, GLsizei height, GLsizei levels, GLenum internalFormat);

        void storage3D(GLsizei width, GLsizei height, GLsizei depth, GLsizei levels, GLenum internalFormat);

        void storage3DSparse(GLsizei width, GLsizei height, GLsizei depth, GLsizei levels, GLenum internalFormat);

        void compressedSubmiage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);

        void
        subimage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type,
                   const void *data);

        void submimage3D(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height,
                         GLsizei depth, GLenum format, GLenum type, const void *data);

        TextureHandle getHandle();

        TextureHandle getHandle(const Sampler& sampler);

        ImageHandle getImageHandle(GLint level, bool layered, GLint layer, GLenum format);

        explicit Texture(GLenum target);

        ~Texture();

        void invalidate(GLint level);

        void invalidate();

        void parameter(GLenum param, GLint value);

        void parameter(GLenum param, const GLint *values);

        void parameter(GLenum param, GLfloat value);

        void parameter(GLenum param, GLfloat *values);

        int getParameteri(GLenum param);

        void pageCommitment(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, bool commit);

    protected:

        void storage3D_internal(GLsizei width, GLsizei height, GLsizei depth, GLsizei levels, GLenum internalFormat);

        void storage2D_internal(GLsizei width, GLsizei height, GLsizei levels, GLenum internalFormat);

        GLsizei width, height, depth, levels;
    };

}


#endif //WAGL2_TEXTURE_H
