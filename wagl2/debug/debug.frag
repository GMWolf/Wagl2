#version 450 core

in Vertex {
    vec3 colour;
} IN;

layout(location = 0) out vec4 outColour;
void main() {
    outColor = IN.colour;
}
