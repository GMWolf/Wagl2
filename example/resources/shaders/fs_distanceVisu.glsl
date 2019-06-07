#version 450 core
#extension GL_ARB_bindless_texture : require

#include "quaternion.glsl"

in vec2 texCoord;

layout(location = 0) out vec4 outcolor;

layout(binding = 0) uniform SceneInfo {
    mat4 VP;
    vec3 viewPos;
};

struct Box {
    vec3 min; uint pad1;
    vec3 max; uint pad2;
};

struct Transform {
    vec3 pos;
    float scale;
    vec4 rot;
};

layout(binding = 1) uniform TransformBlock {
    Transform transform;
};

layout(binding = 2) uniform DistanceTex {
    Box bbox;
    sampler3D sdfTex;
};

float sdBox( vec3 p, vec3 b )
{
    vec3 d = abs(p) - b;
    return length(max(d,0.0))
    + min(max(d.x,max(d.y,d.z)),0.0); // remove this line for an only partially signed sdf
}

void swap(in out float a, in out float b) {
    float t = a;
    a = b;
    b = t;
}

//Christer Ericson Real-Time Collision Detection
bool intersectRayAABB(vec3 p, vec3 d, Box a, out float tmin, out vec3 q) {
    const float epsilon = 0.0001;
    tmin = 0.0f;
    float tmax = 100000;
    for(int i = 0; i < 3; i++) {
        if(abs(d[i]) < epsilon) {
            if (p[i] < a.min[i] || p[i] > a.max[i]) return false;
        } else {
            float ood  = 1.0f / d[i];
            float t1 = (a.min[i] - p[i]) * ood;
            float t2 = (a.max[i] - p[i]) * ood;
            if (t1 > t2) swap(t1, t2);
            if (t1 > tmin) tmin = t1;
            if (t2 < tmax) tmax = t2;
            if (tmin > tmax) return false;
        }
    }

    q = p + d * tmin;
    return true;
}

vec3 invTransform(vec3 p, Transform t) {
    return invrotate(p - t.pos, t.rot) / t.scale;
}

void main() {

    vec4 r = inverse(VP) * vec4(texCoord * 2.0 - 1.0, 1.0, 1.0);
    r /= r.w;
    r.xyz -= viewPos;
    r.xyz = normalize(r.xyz);

    //put viewpos and ray in object space
    vec3 rp = invTransform(viewPos, transform);
    vec3 rd = invrotate(r.xyz, transform.rot);

    float boxTmin;
    vec3 boxIPos;
    bool intersect = intersectRayAABB(rp, rd, bbox, boxTmin, boxIPos);

    if (intersect) {
        rp = rp + rd * boxTmin;
        float dist = boxTmin;
        const float maxDist = 5;
        for(int i = 0; i < 128; i++) {
            vec3 tp = (rp - bbox.min) / (bbox.max - bbox.min);
            if (i != 0 && (any(greaterThan(tp, vec3(1))) || any(lessThan(tp, vec3(0))))) {
                outcolor = vec4(1);
                return;
            }
            float d = texture(sdfTex, tp).x;
            //d += max(0, sdBox(rp, vec3(1)));

            rp += rd * d;
            dist += d;
            if (d < 0.04) {
                break;
            }
        }
        outcolor = vec4(vec3(dist/8.0),1);
    } else {
        outcolor = vec4(1);
    }

}