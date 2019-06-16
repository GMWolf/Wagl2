#version 450
#extension GL_ARB_bindless_texture : require

#include "common.cshared"
#include "PBR.glsl"
#include "normals.glsl"

in Vertex {
    vec3 normal;
    vec2 texCoord;
    vec3 view;
} IN;


layout(binding = TEXTURE_BLOCK_BINDING) uniform Textures {
    sampler2D albedo;
    sampler2D metal;
    sampler2D roughness;
    sampler2D normal;

    sampler2D commitment;
} textures;

float availableLod(vec2 uv) {
    /* texture gather only works if commitment tex resolution mathced other tex
    vec4 g = textureGather(textures.commitment, uv);
    return max(max(g.x, g.y), max(g.z, g.w));
    */
    return texture(textures.commitment, uv).r;
}

vec4 sampleSparse(sampler2D tex, vec2 uv, float maxLod) {
    vec2 lodQ = textureQueryLod(tex, uv);
    float lod = max(maxLod, lodQ.x);
    return textureLod(tex, uv, lod);
}

PBRFragment samplePBR() {
    float maxLod = availableLod(IN.texCoord);

    vec4 albedo = sampleSparse(textures.albedo, IN.texCoord, maxLod);
    vec4 metal = sampleSparse(textures.metal, IN.texCoord, maxLod);
    vec4 normal = sampleSparse(textures.normal, IN.texCoord, maxLod);
    vec4 roughness = sampleSparse(textures.roughness, IN.texCoord, maxLod);

    //rebuild normals
    normal = normal * 2.0 - 1.0;
    normal.z = sqrt(1 - dot(normal.xy, normal.xy));
    normal.xy = normal.yx;
    normal.y *= -1;

    vec3 N = normalize(IN.normal);

    PBRFragment f;
    f.albedo = albedo.xyz;
    f.metalicity = metal.r;
    f.roughness = roughness.r;
    f.emmisivity = vec3(0,0,0);
    f.normal = perturb_normal(N, IN.view, IN.texCoord, normal.xyz );
    //f.normal = vec3(0,1,0);
    f.AO = 1;

    return f;
}

layout(location = 0) out vec4 outColor;
void main() {

    PBRFragment f = samplePBR();
    LightFragment l;
    l.lightDirection = normalize(vec3(1, 1, 0));
    l.radiance = vec3(1) * 5;

    outColor.rgb = color(f, l, normalize(IN.view));
    //outColor.rgb = 0.1f + texture(textures.commitment, IN.texCoord).rgb / 8.0f;
    //outColor.rgb = f.normal;
    outColor.a = 1;
}
