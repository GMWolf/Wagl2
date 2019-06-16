#version 450 core
#extension GL_ARB_bindless_texture : require

layout(binding = 0) uniform Textures {
    sampler2D albedo;
    sampler2D metal;
    sampler2D roughness;
    sampler2D normal;
} textures;

layout (binding = 1) uniform InfoBlock {
    vec2 offset;
    vec2 size;
    int lod;
};

in vec2 texCoord;

layout(location = 0) out vec3 outAlbedo;
layout(location = 1) out vec2 outNormal;
layout(location = 2) out float outRoughness;
layout(location = 3) out float outMetallic;
void main() {

    vec2 uv = (offset + (texCoord * pow(2,lod))) * size;
    outAlbedo = textureLod(textures.albedo, uv, lod).rgb;
    outNormal = textureLod(textures.normal, uv, lod).xy;
    outRoughness = textureLod(textures.roughness, uv, lod).x;
    outMetallic = textureLod(textures.metal, uv, lod).x;
}
