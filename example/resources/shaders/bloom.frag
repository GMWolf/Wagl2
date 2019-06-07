#version 450 core
#extension GL_ARB_bindless_texture : require

in vec2 texCoord;

layout(location = 0) out vec4 outcolor;

layout(binding = 0) uniform InBlock {
    sampler2D color;
    sampler2D bright;
};

void main() {
    outcolor.rgb = texture(color, texCoord).rgb;
    outcolor.rgb += texture(bright, texCoord).rgb * 0.5;
    outcolor.a = 1.0;
}