//
// Created by felix on 5/4/2019.
//

#ifndef WAGL2_FULLSCREEN_H
#define WAGL2_FULLSCREEN_H

#include <glad/glad.h>
#include <tuple>
#include <string>

namespace wagl::shd {
        const std::pair<GLenum, std::string> vs_fullscreen = { GL_VERTEX_SHADER,
            R"(#version 430 core
                out vec2 texCoord;
                void main()
                {
                    float x = -1.0 + float((gl_VertexID & 1) << 2);
                    float y = -1.0 + float((gl_VertexID & 2) << 1);
                    texCoord.x = (x+1.0)*0.5;
                    texCoord.y = (y+1.0)*0.5;
                    gl_Position = vec4(x, y, 0, 1);
                }
            )"
        };
    }

#endif //WAGL2_FULLSCREEN_H
