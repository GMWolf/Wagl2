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
    sampler2D metalRoughness;
    sampler2D emissive;
    sampler2D normal;
    sampler2D AO;
} textures;

layout(location = 0) out vec4 outColor;
//layout(location = 1) out vec4 outBright;

void main() {

    vec4 metalRoughness = texture(textures.metalRoughness, IN.texCoord);
    float metal = metalRoughness.b;
    float roughness = metalRoughness.g;
    vec3 albedo = texture(textures.albedo, IN.texCoord).xyz;
    vec3 emissive = texture(textures.emissive, IN.texCoord).xyz;
    float AO = texture(textures.AO, IN.texCoord).x;
    vec3 normalMap = texture(textures.normal, IN.texCoord).xyz * 2.0 - 1.0;
    normalMap.y *= -1;

    vec3 L = normalize(vec3(1, 1, 0)); //light input

    vec3 N = normalize(IN.normal);
    N = perturb_normal(N, IN.view, IN.texCoord, normalMap);

    vec3 V = normalize(IN.view);
    vec3 H = normalize(L + V);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metal);

    vec3 F = fresnel(max(dot(H, V), 0.0), F0);
    float NDF = D_GGX(N, H, roughness);
    float G = G_smith(N, V, L, roughness);
    vec3 numerator = NDF * G * F;

    float denom = 4.0 * max(dot(N,V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denom, 0.00001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metal;

    float NdotL = max(dot(N, L), 0.0);
    vec3 radiance = vec3(5,5,5) * AO ;
    outColor.rgb = (kD * albedo / PI + specular) * radiance * NdotL;
    outColor.rgb += emissive * 5.0;
    outColor.rgb += albedo * 0.03 * AO;
    outColor.a = 1.0;

    float brightness = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722));
   /* if (brightness > 1) {
        outBright = vec4(outColor.rgb, 1.0);
    } else {
        outBright = vec4(0,0,0,1.0);
    }*/

    //outColor.rgb = vec3(roughness);
}
