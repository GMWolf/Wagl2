#ifndef QUATERNION
#define QUATERNION

vec3 rotate(vec3 vec, vec4 quat) {
    vec3 t = 2 * cross(quat.xyz, vec);
    return vec + quat.w * t + cross(quat.xyz, t);
}

vec3 invrotate(vec3 vec, vec4 quat) {
    vec4 q = vec4(-quat.xyz, quat.w);
    return rotate(vec, q);
}

#endif //QUATERNION