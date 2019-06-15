#version 450 core
#extension GL_ARB_bindless_texture : require

#include "common.cshared"

layout(binding = 0) uniform Textures {
    sampler2D albedo;
    sampler2D metal;
    sampler2D roughness;
    sampler2D normal;
    sampler2D emissive;
} textures;

layout(binding = 1) uniform Images {
    writeonly image2D albedo[8];
    writeonly image2D metal[8];
    writeonly image2D roughness[8];
    writeonly image2D normal[8];
    writeonly image2D emissive[8];
} images;

layout (binding = 2) uniform InfoBlock {
  ivec2 offset;
  ivec2 size;
  int lod;
};


layout(local_size_x = 1, local_size_y = 1) in;
void main() {
    ivec2 pos = offset + ivec2(gl_GlobalInvocationID.xy);
    pos = ivec2(mod(pos, textureSize(textures.albedo, lod)));

    vec4 lodColor = vec4(0,0,0,0);
    if (lod == 0) {
        lodColor = vec4(1,0,0,1);
    } else if (lod == 1) {
        lodColor = vec4(0,1,0,1);
    } else if (lod == 2) {
        lodColor = vec4(0,0,1,1);
    } else  if (lod == 3) {
        lodColor = vec4(1,0,1,1);
    } else  if (lod == 4) {
        lodColor = vec4(0,1,1,1);
    } else if (lod == 5) {
        lodColor = vec4(1,1,1,1);
    } else  if (lod == 6) {
        lodColor = vec4(0.5,0.5,0.5,1);
    }

    imageStore(images.albedo[lod], pos, lodColor)/*texelFetch(textures.albedo, pos, lod) */ ;
    imageStore(images.metal[lod], pos, texelFetch(textures.metal, pos, lod));
    imageStore(images.roughness[lod], pos, texelFetch(textures.roughness, pos, lod));
    imageStore(images.normal[lod], pos, texelFetch(textures.normal, pos, lod));
    imageStore(images.emissive[lod], pos, texelFetch(textures.emissive, pos, lod));
}
