#version 450 core
#extension GL_ARB_bindless_texture : require

layout(binding = 0) uniform Image {
    layout(r32f) image3D dest;
};

layout (binding = 1) buffer MeshElements {
    vec3 bmin; uint pad0;
    vec3 bmax; uint pad1;
    uint triangleCount;
    uint pad2,pad3,pad4;
    vec4 positions[];
};


float dot2( in vec3 v ) { return dot(v,v); }

float sdfTriangle( vec3 p, vec3 a, vec3 b, vec3 c )
{
    vec3 ba = b - a; vec3 pa = p - a;
    vec3 cb = c - b; vec3 pb = p - b;
    vec3 ac = a - c; vec3 pc = p - c;
    vec3 nor = cross(ba, ac);

    return sqrt(
    (sign(dot(cross(ba, nor), pa)) +
    sign(dot(cross(cb, nor), pb)) +
    sign(dot(cross(ac, nor), pc))<2.0)
    ?
    min(min(
    dot2(ba*clamp(dot(ba, pa)/dot2(ba), 0.0f, 1.0f)-pa),
    dot2(cb*clamp(dot(cb, pb)/dot2(cb), 0.0f, 1.0f)-pb)),
    dot2(ac*clamp(dot(ac, pc)/dot2(ac), 0.0f, 1.0f)-pc))
    :
    dot(nor, pa)*dot(nor, pa)/dot2(nor) );
}


float sdfMesh(vec3 p) {
    float d = sdfTriangle(p, positions[0].xyz, positions[1].xyz, positions[2].xyz);
    for(int i = 1; i < triangleCount; i++) {
        vec3 a = positions[i * 3 + 0].xyz;
        vec3 b = positions[i * 3 + 1].xyz;
        vec3 c = positions[i * 3 + 2].xyz;
        float _d = sdfTriangle(p, a, b, c);
        if (_d < d) {
            d = _d;
        }
    }

    return d;
}

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {

    vec3 p = mix(bmin, bmax, vec3(gl_GlobalInvocationID.xyz) / vec3(gl_NumWorkGroups * gl_WorkGroupSize));
    float d = sdfMesh(p);
    //float d = gl_GlobalInvocationID.x / vec3(gl_NumWorkGroups * gl_WorkGroupSize).x;
    //float d = sdfTriangle(p, positions[0], positions[1], positions[2]);
    imageStore(dest, ivec3(gl_GlobalInvocationID.xyz), vec4(d,d,d,1));
}
