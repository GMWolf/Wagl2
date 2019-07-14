#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(binding = 0) uniform GenInfo {
    mat4 proj;
};

out Vertex {
    vec3 normal;
    vec2 texCoord;
} OUT;

void main() {
    gl_Position = proj * vec4(position, 1.0);
    OUT.normal = normal;
    OUT.texCoord = texCoord;
}
