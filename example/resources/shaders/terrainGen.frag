#version 450 core
#extension GL_ARB_bindless_texture : require

#include "textureNoTile.glsl"

layout(binding = 0) uniform Textures {
    sampler2D albedo;
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
layout(location = 1) out vec4 outRoughMetalNormal;
void main() {
    vec2 uv = (offset + (texCoord * pow(2,lod))) * size;
    //outAlbedo = textureLod(textures.albedo, uv, lod).rgb;
    //outRoughMetalNormal.r = textureLod(textures.roughness, uv, lod).x;
    //outRoughMetalNormal.g = 0.0;//textureLod(textures.metal, uv, lod).x;
    //outRoughMetalNormal.ba = textureLod(textures.normal, uv, lod).xy;
    outAlbedo = textureNoTile(textures.albedo, uv, 0.6).rgb;
    outRoughMetalNormal.r = textureNoTile(textures.roughness, uv, 0.6).r;
    outRoughMetalNormal.g = 0.0;
    outRoughMetalNormal.ba = textureNoTile(textures.normal, uv, 0.6).xy;
    //outAlbedo = texture(textures.albedo, uv).rgb;
    //outRoughMetalNormal.r = texture(textures.roughness, uv).x;
    //outRoughMetalNormal.g = 0.0;
    //outRoughMetalNormal.ba = texture(textures.normal, uv).xy;
}
