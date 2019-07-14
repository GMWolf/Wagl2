#version 450 core

#extension GL_ARB_bindless_texture : require

//Simple copy-to-buffer shader. assumes 4 bytes per pixel.

layout(binding = 0) uniform Textures {
    sampler2D tex;
    int lod;
};

layout (std430, binding = 1) buffer InfoBlock {
    vec4 data[];
};

layout(local_size_x = 1, local_size_y = 1) in;
void main() {

    uvec2 id = gl_GlobalInvocationID.xy;
    uvec2 imageSize = gl_WorkGroupSize.xy * gl_NumWorkGroups.xy;
    uint pixelIndex = id.x + (id.y * imageSize.x);

    uint dataOffset = pixelIndex;
    data[dataOffset] = texelFetch(tex, ivec2(id), lod);
}
