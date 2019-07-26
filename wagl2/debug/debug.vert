#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colour;

layout(binding = CAMERA_BINDING) uniform ProjectionBLock {
    mat4 VP;
};

out Vertex {
    vec3 colour;
} OUT;

void main() {

    gl_Position = VP * vec4(position, 1.0);
    OUT.colour = colour;
}