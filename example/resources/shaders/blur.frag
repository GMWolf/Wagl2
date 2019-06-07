#version 450 core
#extension GL_ARB_bindless_texture : require

in vec2 texCoord;

layout(location = 0) out vec4 outcolor;

layout(binding = 0) uniform InBlock {
    sampler2D color;
    vec2 direction;
};

//values from http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
const float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
const float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main() {
    outcolor = texture(color, texCoord) * weight[0];
    for(int i = 1; i < 3; i++) {
        const vec2 off = direction * offset[i] / textureSize(color, 0);
        outcolor += texture(color, texCoord + off) * weight[i];
        outcolor += texture(color, texCoord - off) * weight[i];
    }

}