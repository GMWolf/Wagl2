#version 450 core
#extension GL_ARB_bindless_texture : require

in vec2 texCoord;

layout(location = 0) out vec4 outcolor;

layout(binding = 2) uniform Textures {
  sampler2D colour;
} ;

void main() {
    outcolor = texture(colour, texCoord);
}