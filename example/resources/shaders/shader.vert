#version 450 core

#include "common.cshared"
#include "quaternion.glsl"

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(binding = SCENE_INFO_BINDING) uniform SceneInfo {
    mat4 VP;
    vec3 viewPos;
};

struct Transform {
    vec3 pos;
    float scale;
    vec4 rot;
};

layout(binding = TRANSFORM_BLOCK_BINDING) uniform TransformBlock {
    Transform transform;
};

out Vertex {
    vec3 normal;
    vec2 texCoord;
    vec3 view;
} OUT;

void main() {

    vec3 transformedPos = (rotate(position, transform.rot) * transform.scale) + transform.pos;

    gl_Position = VP * vec4(transformedPos, 1.0);
    OUT.normal = rotate(normal, transform.rot);
    OUT.texCoord = texCoord;
    OUT.view = viewPos - transformedPos;
}
