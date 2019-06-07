//
// Created by felix on 5/5/2019.
//

#ifndef WAGL2_TEXTURELOAD_H
#define WAGL2_TEXTURELOAD_H

#include <gl/Texture.h>
#include <string>

namespace wagl {

    gl::Texture loadTexture(const std::string& fileName);

}


#endif //WAGL2_TEXTURELOAD_H
