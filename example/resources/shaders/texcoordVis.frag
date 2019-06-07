#version 450 core

in vec2 texCoord;

layout(location = 0) out vec4 outcolor;

void main() {
    outcolor = vec4(texCoord, 1.0, 1.0);
}
