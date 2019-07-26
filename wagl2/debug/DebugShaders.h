//
// Created by felix on 26/07/2019.
//

#ifndef CODE_DEBUGSHADERS_H
#define CODE_DEBUGSHADERS_H

#include <tuple>
#include <glad/glad.h>

namespace wagl::shd {

    const std::pair<GLenum, std::string> vs_debug = {
            GL_VERTEX_SHADER,
            R"(
#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colour;

layout(binding = 0) uniform ProjectionBLock {
    mat4 VP;
};

out Vertex {
    vec3 colour;
} OUT;

void main() {

    gl_Position = VP * vec4(position, 1.0);
    OUT.colour = colour;
}

)"};

    const std::pair<GLenum, std::string> fs_debug = {
            GL_FRAGMENT_SHADER,
            R"(
#version 450 core
in Vertex {
    vec3 colour;
} IN;

layout(location = 0) out vec4 outColour;
void main() {
    outColour = vec4(IN.colour, 1);
}
)"};

}
#endif //CODE_DEBUGSHADERS_H
