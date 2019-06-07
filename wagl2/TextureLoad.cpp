//
// Created by felix on 5/5/2019.
//

#include "TextureLoad.h"

#include <gli/gli.hpp>

wagl::gl::Texture wagl::loadTexture(const std::string &fileName) {

    gli::texture gliTex = gli::load(fileName.c_str());
    if(gliTex.empty()) {
        //TODO return something here
    }

    gli::gl GL(gli::gl::PROFILE_GL33);
    const gli::gl::format format = GL.translate(gliTex.format(), gliTex.swizzles());
    GLenum target = GL.translate(gliTex.target());

    gl::Texture texture(target);
    texture.parameter(GL_TEXTURE_BASE_LEVEL, 0);
    texture.parameter(GL_TEXTURE_MAX_LEVEL, GLint(gliTex.levels() - 1));
    texture.parameter(GL_TEXTURE_SWIZZLE_RGBA, &format.Swizzles[0]);
    texture.storage2D(gliTex.extent(0).x, gliTex.extent(0).y, gliTex.levels(), format.Internal);

    for(size_t level = 0; level < gliTex.levels(); level++) {
        auto extent = gliTex.extent(level);
        if(gli::is_compressed(gliTex.format())) {
            texture.compressedSubmiage2D(level, 0, 0, extent.x, extent.y, format.Internal, gliTex.size(level), gliTex.data(0, 0, level));
        } else {
            texture.subimage2D(level, 0, 0, extent.x, extent.y, format.External, format.Type, gliTex.data(0, 0, level));
        }
    }

    return texture;
}
