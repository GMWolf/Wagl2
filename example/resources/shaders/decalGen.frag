#version 450 core

#extension GL_ARB_bindless_texture : require

in Vertex {
    vec3 normal;
    vec2 texCoord;
} IN;

layout(binding = 1) uniform Textures {
    sampler2D albedo;
    sampler2D alpha;
    sampler2D normal;
    sampler2D roughness;
} textures;

layout(location = 0) out vec3 outAlbedo;
layout(location = 1) out vec4 outRoughMetalNormal;
void main() {

    float alpha = texture(textures.alpha, IN.texCoord).r;
    if (alpha <= 0.0)
        discard;

    outAlbedo = texture(textures.albedo, IN.texCoord).rgb;
    outRoughMetalNormal.r = texture(textures.roughness, IN.texCoord).r;
    outRoughMetalNormal.g = 0.0;
    outRoughMetalNormal.ba = texture(textures.normal, IN.texCoord).xy;
}
