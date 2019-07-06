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
#include "Quadtree.h"

struct SceneInfo {
    glm::mat4 VP;
    glm::vec3 viewPos;
};

struct TerrainTextures {
    wagl::gl::TextureHandle albedo;
    wagl::gl::TextureHandle metalRoughNormal;

    wagl::gl::TextureHandle commitment;
};

struct GenTexture {
    wagl::gl::TextureHandle albedo;
    wagl::gl::TextureHandle metal;
    wagl::gl::TextureHandle roughness;
    wagl::gl::TextureHandle normal;
};

struct GenerationInfo {
    glm::vec2 offset;
    glm::vec2 size;
    int lod;
};

/**
 * Gets page coordinate in pages.
 * @param node
 * @return
 */
glm::uvec2 getPageCoord(uint64_t node) {

    size_t depth = Quadtree::getNodeDepth(node);

    glm::uvec2 coord = {0, 0};

    for(size_t level = 0; level < depth; level++) {
        uint64_t quadrant = (node >> (2 * level));
        glm::uvec2 offset = glm::uvec2{quadrant & 0b01u ? 1 : 0, quadrant & 0b10u ? 1 : 0};
        coord = 2u * coord + offset;
    }

    return coord;
}

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

    wagl::UniformBuffer<GenTexture> genTex;
    genTex->albedo = matTextures.albedo.getHandle();
    genTex->albedo.makeResident();
    genTex->roughness = matTextures.roughness.getHandle();
    genTex->roughness.makeResident();
    genTex->normal = matTextures.normal.getHandle();
    genTex->normal.makeResident();
    genTex->metal = matTextures.metallic.getHandle();
    genTex->metal.makeResident();

    wagl::gl::Shader terrainShader {
            {GL_VERTEX_SHADER, wagl::loadFileGLSL("resources/shaders/shader.vert",  "resources/shaders")},
            {GL_FRAGMENT_SHADER, wagl::loadFileGLSL("resources/shaders/terrain.frag", "resources/shaders")}
    };

    wagl::gl::Shader pipeGenShd {
            wagl::shd::vs_fullscreen,
            {GL_FRAGMENT_SHADER, wagl::loadFileGLSL("resources/shaders/terrainGen.frag", "resources/shaders")}
    };


    glm::uvec2 texSize = {2048 * 4, 2048 * 4};
    size_t texLevels = 5;
    wagl::gl::Texture albedo(GL_TEXTURE_2D);
    albedo.storage2DSparse(texSize, texLevels, GL_R11F_G11F_B10F);
    wagl::gl::Texture metalRoughNormal(GL_TEXTURE_2D);
    metalRoughNormal.storage2DSparse(texSize, texLevels, GL_RGBA8);

    glm::uvec2 pageSize = albedo.getPageSize();
    pageSize = glm::max(pageSize, glm::uvec2(metalRoughNormal.getPageSize()));

    wagl::gl::Texture commitment(GL_TEXTURE_2D);
    commitment.storage2D(texSize / pageSize, 1, GL_R32F);

    wagl::gl::FrameBuffer frameBuffers[texLevels];
    for(size_t i = 0; i < texLevels; i++) {
        frameBuffers[i].texture(GL_COLOR_ATTACHMENT0, albedo, i);
        frameBuffers[i].texture(GL_COLOR_ATTACHMENT1, metalRoughNormal, i);
        frameBuffers[i].drawBuffers({
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1
        });
    }

    Quadtree q;
    std::vector<uint64_t> batchAdd;

    auto commitPage = [&](uint64_t node) {
        q.add(node);
        auto pageCoord = getPageCoord(node);
        auto texelCoord = pageCoord * pageSize;
        auto level = texLevels - Quadtree::getNodeDepth(node) - 1;
        albedo.pageCommitment(level, {texelCoord, 0}, {pageSize, 1}, true);
        metalRoughNormal.pageCommitment(level, {texelCoord, 0}, {pageSize, 1}, true);

        commitment.subClear(0, {pageCoord * (1u << level), 0} , {glm::uvec2(1u << level), 1}, (float)level);
        std::cout << level << std::endl;
        std::cout << (pageCoord * (1u << level)).x << std::endl;
        std::cout << glm::uvec2(1u << level).x << std::endl;
    };

    commitPage(q.rootNodeCode());

    pipeGenShd.use();
    floor.gl.vertexArray.bind();
    wagl::UniformBuffer<GenerationInfo> genInfoBuffer;
    for(int i = 0; i < 10; i++) {
        q.traversePotentialNodes([&](uint64_t node) {
            if (((node & 0b11u) == 0) && Quadtree::getNodeDepth(node) < texLevels) {
                batchAdd.push_back(node);
            }
        });

        for(auto p : batchAdd) {
            commitPage(p);
            uint64_t level = texLevels - Quadtree::getNodeDepth(p) - 1;

            frameBuffers[level].bind();

            genInfoBuffer.invalidate();
            auto coord = getPageCoord(p);
            genInfoBuffer->lod = level;
            genInfoBuffer->offset = coord;
            genInfoBuffer->size = glm::vec2(1);
            genInfoBuffer.bind(1);
            genTex.bind(0);

            glViewport(coord.x * pageSize.x, coord.y * pageSize.y, pageSize.x, pageSize.y);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            std::cout << std::bitset<64>(p) << std::endl;
        }

        batchAdd.clear();
    }

    wagl::gl::FrameBuffer::unbind();

    wagl::UniformBuffer<wagl::Transform> transformBlock;
    *transformBlock = {
            {0,0,0},
            1,
            glm::quat()
    };

    wagl::UniformBuffer<SceneInfo> sceneInfoBuffer;

    wagl::UniformBuffer<TerrainTextures> terrainTexturesBuffer;
    terrainTexturesBuffer->albedo = albedo.getHandle(sampler);
    terrainTexturesBuffer->metalRoughNormal = metalRoughNormal.getHandle(sampler);
    terrainTexturesBuffer->commitment =  commitment.getHandle(nearestSampler);
    terrainTexturesBuffer->albedo.makeResident();
    terrainTexturesBuffer->metalRoughNormal.makeResident();
    terrainTexturesBuffer->commitment.makeResident();

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