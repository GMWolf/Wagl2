//
// Created by felix on 07/06/2019.
//

#include <Application.h>
#include <Camera.h>
#include "CameraController.h"
#include <Model.h>
#include <TextureLoad.h>
#include <gl/Shader.h>
#include <util/files.h>
#include <common.cshared>
#include <UniformBuffer.h>
#include <iostream>
#include <gl/FrameBuffer.h>
#include "VirtualTextureMap.h"
#include <shaders/fullscreen.h>

struct PBRMaterial {
    wagl::gl::TextureHandle albedo;
    wagl::gl::TextureHandle metalRoughness;
    wagl::gl::TextureHandle normal;
    wagl::gl::TextureHandle ao;
};

struct SceneInfo {
    glm::mat4 VP;
    glm::vec3 viewPos;
};

struct TerrainTextures {
    wagl::gl::TextureHandle albedo;
    wagl::gl::TextureHandle metal;
    wagl::gl::TextureHandle roughness;
    wagl::gl::TextureHandle normal;

    wagl::gl::TextureHandle commitment;
};

struct GenTexture {
    wagl::gl::TextureHandle albedo;
    wagl::gl::TextureHandle metal;
    wagl::gl::TextureHandle roughness;
    wagl::gl::TextureHandle normal;
};

template<size_t LEVELS>
struct TerrainImages {
    wagl::gl::ImageHandle albedo[LEVELS];
    wagl::gl::ImageHandle metal[LEVELS];
    wagl::gl::ImageHandle roughness[LEVELS];
    wagl::gl::ImageHandle normal[LEVELS];

    wagl::gl::ImageHandle commitment;
};

struct GenerationInfo {
    glm::vec2 offset;
    glm::vec2 size;
    int lod;
};

void run() {

    wagl::Application app({
        1280, 720,
        "Virtual Texturing"
    });

    wagl::Camera camera;
    camera.fovy = glm::radians(45.0f);
    camera.zfar = 1000;
    camera.znear = 0.1;
    camera.transform.position = {-100, 1.75, -100};
    camera.transform.orientation = glm::quatLookAt(glm::vec3{0,0,-1}, {0,1,0});

    CameraController cameraController {app.window, camera};

    wagl::Mesh floor;
    floor.data.vertices = {
            {{-100, 0, -100}, {0, 1, 0}, {0,0}},
            {{-100, 0, 100}, {0, 1, 0}, {0, 1}},
            {{100, 0, -100}, {0, 1, 0}, {1, 0}},
            {{100, 0, 100}, {0, 1, 0}, {1, 1}}
    };
    floor.data.elements = {
            0, 1, 2, 2, 1, 3
    };
    floor.update();

    wagl::gl::Sampler sampler {
            //{GL_TEXTURE_MAX_ANISOTROPY, 16.0f},
            {GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
            {GL_TEXTURE_MAG_FILTER, GL_LINEAR}
    };

    wagl::gl::Sampler nearestSampler {
            {GL_TEXTURE_MIN_FILTER, GL_NEAREST},
            {GL_TEXTURE_MAG_FILTER, GL_NEAREST}
    };

    struct {
        wagl::gl::Texture albedo = wagl::loadTexture("resources/textures/sci-fi_panel/Scifi_Panel4_1K_albedo.dds");
        wagl::gl::Texture metallic = wagl::loadTexture("resources/textures/sci-fi_panel/Scifi_Panel4_1K_metallic.dds");
        wagl::gl::Texture normal = wagl::loadTexture("resources/textures/sci-fi_panel/Scifi_Panel4_1K_normal.dds");
        wagl::gl::Texture roughness = wagl::loadTexture("resources/textures/sci-fi_panel/Scifi_Panel4_1K_roughness.dds");
    } matTextures {};


    glm::uvec2 virtualTexSize = {2048*8, 2048*8};
    const size_t virtualTexLevels = 8;

    struct {
        wagl::gl::Texture albedo = wagl::gl::Texture(GL_TEXTURE_2D);
        wagl::gl::Texture metallic = wagl::gl::Texture(GL_TEXTURE_2D);
        wagl::gl::Texture normal = wagl::gl::Texture(GL_TEXTURE_2D);
        wagl::gl::Texture roughness = wagl::gl::Texture(GL_TEXTURE_2D);
    } virtualTextures {};
    virtualTextures.albedo.storage2DSparse(virtualTexSize, virtualTexLevels, GL_R11F_G11F_B10F);
    virtualTextures.metallic.storage2DSparse(virtualTexSize, virtualTexLevels, GL_R8);
    virtualTextures.normal.storage2DSparse(virtualTexSize, virtualTexLevels, GL_RG8);
    virtualTextures.roughness.storage2DSparse(virtualTexSize, virtualTexLevels, GL_R8);

    glm::ivec3 pageSize = virtualTextures.albedo.getPageSize();
    pageSize = glm::max(pageSize, virtualTextures.normal.getPageSize());
    pageSize = glm::max(pageSize, virtualTextures.metallic.getPageSize());
    pageSize = glm::max(pageSize, virtualTextures.roughness.getPageSize());

    std::cout << "pageSize " << pageSize.x << " " << pageSize.y << "\n";

    glm::uvec2 virtualCommitmentTexSize = (virtualTexSize + glm::uvec2(pageSize) - 1u) / glm::uvec2(pageSize);

    wagl::gl::Texture commitmentTex = wagl::gl::Texture(GL_TEXTURE_2D);
    commitmentTex.storage2D(virtualCommitmentTexSize.x, virtualCommitmentTexSize.y, 1, GL_R32F);
    commitmentTex.clear(0, 11.0f);



    wagl::gl::Shader terrainShader {
            {GL_VERTEX_SHADER, wagl::loadFileGLSL("resources/shaders/shader.vert",  "resources/shaders")},
            {GL_FRAGMENT_SHADER, wagl::loadFileGLSL("resources/shaders/terrain.frag", "resources/shaders")}
    };

    wagl::gl::Shader genShd {
            {GL_COMPUTE_SHADER, wagl::loadFileGLSL("resources/shaders/csgeneration.glsl", "resources/shaders")}
    };

    wagl::gl::Shader pipeGenShd {
            wagl::shd::vs_fullscreen,
            {GL_FRAGMENT_SHADER, wagl::loadFileGLSL("resources/shaders/terrainGen.frag", "resources/shaders")}
    };

    wagl::UniformBuffer<GenerationInfo> genInfoBuffer;
    wagl::UniformBuffer<GenTexture> genTextureBuffer;
    genTextureBuffer->albedo = matTextures.albedo.getHandle(sampler);
    genTextureBuffer->normal = matTextures.normal.getHandle(sampler);
    genTextureBuffer->metal = matTextures.metallic.getHandle(sampler);
    genTextureBuffer->roughness = matTextures.roughness.getHandle(sampler);
    genTextureBuffer->albedo.makeResident();
    genTextureBuffer->normal.makeResident();
    genTextureBuffer->metal.makeResident();
    genTextureBuffer->roughness.makeResident();

    wagl::UniformBuffer<TerrainTextures> terrainTexturesBuffer;
    terrainTexturesBuffer->albedo = virtualTextures.albedo.getHandle(sampler);
    terrainTexturesBuffer->normal = virtualTextures.normal.getHandle(sampler);
    terrainTexturesBuffer->metal = virtualTextures.metallic.getHandle(sampler);
    terrainTexturesBuffer->roughness = virtualTextures.roughness.getHandle(sampler);
    terrainTexturesBuffer->commitment = commitmentTex.getHandle(nearestSampler);

    terrainTexturesBuffer->albedo.makeResident();
    terrainTexturesBuffer->normal.makeResident();
    terrainTexturesBuffer->metal.makeResident();
    terrainTexturesBuffer->roughness.makeResident();
    terrainTexturesBuffer->commitment.makeResident();

    wagl::gl::FrameBuffer virtFrameBuffers[virtualTexLevels];

    wagl::UniformBuffer<TerrainImages<virtualTexLevels>> terrainImagesBuffer;
    for(int i = 0; i < virtualTexLevels; i++) {
        //terrainImagesBuffer->albedo[i] = virtualTextures.albedo.getImageHandle(i, false, 0, GL_RGBA8);
        //terrainImagesBuffer->albedo[i].makeResident(GL_WRITE_ONLY);
        virtFrameBuffers[i].texture(GL_COLOR_ATTACHMENT0, virtualTextures.albedo, i);
        //terrainImagesBuffer->normal[i] = virtualTextures.normal.getImageHandle(i, false, 0, GL_RG8);
        //terrainImagesBuffer->normal[i].makeResident(GL_WRITE_ONLY);
        virtFrameBuffers[i].texture(GL_COLOR_ATTACHMENT1, virtualTextures.normal, i);
        //terrainImagesBuffer->roughness[i] = virtualTextures.roughness.getImageHandle(i, false, 0, GL_R8);
        //terrainImagesBuffer->roughness[i].makeResident(GL_WRITE_ONLY);
        virtFrameBuffers[i].texture(GL_COLOR_ATTACHMENT2, virtualTextures.roughness, i);
        //terrainImagesBuffer->metal[i] = virtualTextures.metallic.getImageHandle(i, false, 0, GL_R8);
        //terrainImagesBuffer->metal[i].makeResident(GL_WRITE_ONLY);
        virtFrameBuffers[i].texture(GL_COLOR_ATTACHMENT3, virtualTextures.metallic, i);
        virtFrameBuffers[i].drawBuffers({
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
        });
    }
    terrainImagesBuffer->commitment = commitmentTex.getImageHandle(0, false, 0, GL_R32F);
    terrainImagesBuffer->commitment.makeResident(GL_READ_WRITE);

    auto virtTexCommitment = [&](glm::uvec2 p, glm::uvec2 size, int level, bool commitment) {
        virtualTextures.albedo.pageCommitment(level, p.x, p.y, 0, size.x, size.y, 1, commitment);
        //virtualTextures.emissive.pageCommitment(level, p.x, p.y, 0, size.x, size.y, 1, commitment);
        virtualTextures.normal.pageCommitment(level, p.x, p.y, 0, size.x, size.y, 1, commitment);
        virtualTextures.roughness.pageCommitment(level, p.x, p.y, 0, size.x, size.y, 1, commitment);
        virtualTextures.metallic.pageCommitment(level, p.x, p.y, 0, size.x, size.y, 1, commitment);

        glm::uvec2 cpos = (p / glm::uvec2(pageSize)) * (unsigned int)(glm::pow(2, level));
        glm::uvec2 csize =( (size + glm::uvec2(pageSize) - 1u) / glm::uvec2(pageSize)) * (unsigned int) (glm::pow(2, level));
        commitmentTex.subClear(0, {cpos,0}, {csize, 1}, (float)level);
    };

    glm::ivec3 maxGroupSize;
    glGetIntegeri_v(GL_MAX_COMPUTE_VARIABLE_GROUP_SIZE_ARB, 0, &maxGroupSize.x);
    glGetIntegeri_v(GL_MAX_COMPUTE_VARIABLE_GROUP_SIZE_ARB, 1, &maxGroupSize.y);
    glGetIntegeri_v(GL_MAX_COMPUTE_VARIABLE_GROUP_SIZE_ARB, 2, &maxGroupSize.z);
    GLint maxInv;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS , &maxInv);

    int mintail = 8;
    mintail = glm::min(mintail, virtualTextures.albedo.getParameteri(GL_NUM_SPARSE_LEVELS_ARB));
    mintail = glm::min(mintail, virtualTextures.roughness.getParameteri(GL_NUM_SPARSE_LEVELS_ARB));
    mintail = glm::min(mintail, virtualTextures.metallic.getParameteri(GL_NUM_SPARSE_LEVELS_ARB));
    mintail = glm::min(mintail, virtualTextures.normal.getParameteri(GL_NUM_SPARSE_LEVELS_ARB));

    //todo commit the tail

    floor.gl.vertexArray.bind();
    for(int i = mintail - 1; i >= 0; i--) {
        std::cout << i << std::endl;
        virtTexCommitment({0, 0}, glm::uvec2(pageSize), i, true);
        /*genInfoBuffer.invalidate();
        genInfoBuffer->size = glm::ivec2(pageSize);
        genInfoBuffer->offset = {0, 0};
        genInfoBuffer->lod = i;
        genShd.use();
        genInfoBuffer.bind(2);
        terrainImagesBuffer.bind(1);
        genTextureBuffer.bind(0);
        glDispatchCompute(pageSize.x, pageSize.y, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);*/
        virtFrameBuffers[i].bind();
        glViewport(0, 0, pageSize.x, pageSize.y);
        pipeGenShd.use();
        genInfoBuffer.invalidate();
        genInfoBuffer->size = glm::vec2(pageSize) / 512.0f;
        genInfoBuffer->offset = {0, 0};
        genInfoBuffer->lod = i;
        genTextureBuffer.bind(0);
        genInfoBuffer.bind(1);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    wagl::gl::FrameBuffer::unbind();
    //virtTexCommitment({0,0}, glm::uvec2(pageSize), 7, true);
    //virtTexCommitment({0,0}, glm::uvec2(pageSize), 6, true);
    //virtTexCommitment({0,0}, glm::uvec2(pageSize), 5, true);
    //virtTexCommitment({0,0}, glm::uvec2(pageSize), 4, true);

    wagl::UniformBuffer<wagl::Transform> transformBlock;
    *transformBlock = {
            {0,0,0},
            1,
            glm::quat()
    };

    wagl::UniformBuffer<SceneInfo> sceneInfoBuffer;

    app.run([&](float dt) {
        cameraController.updateCamera(dt);

        glViewport(0, 0, app.window.getWidth(), app.window.getHeight());
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glClearColor(0,0,0,1);
        glClearDepth(1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sceneInfoBuffer.invalidate();
        sceneInfoBuffer->VP = camera.viewProjection(app.window.getAspect());
        sceneInfoBuffer->viewPos = camera.transform.position;

        terrainShader.use();
        sceneInfoBuffer.bind(SCENE_INFO_BINDING);
        transformBlock.bind(TRANSFORM_BLOCK_BINDING);
        terrainTexturesBuffer.bind(TEXTURE_BLOCK_BINDING);


        floor.gl.vertexArray.bind();
        glDrawElements(GL_TRIANGLES, floor.gl.count, floor.gl.elementType, (void*)floor.gl.first);
    });
}

int main() {
    wagl::init();
    run();
    wagl::termintate();

    return 0;
}