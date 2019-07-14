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
#include "Geom.h"
#include <random>
#include <glm/gtx/rotate_vector.hpp>

struct SceneInfo {
    glm::mat4 VP;
    glm::vec3 viewPos;
};

struct TerrainTextures {
    wagl::gl::TextureHandle albedo;
    wagl::gl::TextureHandle metalRoughNormal;

    wagl::gl::TextureHandle commitment;

    float levels;
};

struct GenTexture {
    wagl::gl::TextureHandle albedo;
    wagl::gl::TextureHandle roughness;
    wagl::gl::TextureHandle normal;
};

struct GenerationInfo {
    glm::vec2 offset;
    glm::vec2 size;
    int lod;
};

struct TexToBufferInfo {
    wagl::gl::TextureHandle tex;
    int lod;
};

struct DecalTextures {
    wagl::gl::TextureHandle albedo;
    wagl::gl::TextureHandle alpha;
    wagl::gl::TextureHandle normal;
    wagl::gl::TextureHandle roughness;
};

struct DecalGenInfo {
    glm::mat4 proj;
};

thread_local std::default_random_engine randomGenerator(0);

/**
 * Gets page coordinate in pages.
 * @param node
 * @return
 */
glm::uvec2 getPageCoord(uint64_t node) {

    if (node == Quadtree::rootNodeCode()) {
        return {0, 0};
    }

    size_t nodeDepth = Quadtree::getNodeDepth(node);

    glm::uvec2 coord = {0, 0};

    for(size_t depth = 0; depth < nodeDepth; depth++) {
        uint64_t quadrant = Quadtree::getLevelCode(node, depth);
        glm::uvec2 offset = glm::uvec2{quadrant & 0b01u ? 1 : 0, quadrant & 0b10u ? 1 : 0};
        coord = 2u * coord + offset;
    }

    return coord;
}

wagl::rect getPageUv(uint64_t node) {
    size_t depth = Quadtree::getNodeDepth(node);
    size_t pageCount = 1 << depth;
    wagl::rect uv;
    uv.min = glm::vec2(getPageCoord(node)) / float(pageCount);
    uv.max = glm::vec2(getPageCoord(node) + 1u) / float(pageCount);
    return uv;
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
    camera.transform.position = {0, 1.75, 0};
    camera.transform.orientation = glm::quatLookAt(glm::vec3{0,0,-1}, {0,1,0});

    CameraController cameraController {app.window, camera};

    wagl::Mesh floor;
    float floorHalfSize = 50;
    floor.data.vertices = {
            {{-floorHalfSize, 0, -floorHalfSize}, {0, 1, 0}, {0,0}},
            {{-floorHalfSize, 0, floorHalfSize}, {0, 1, 0}, {0, 1}},
            {{floorHalfSize, 0, -floorHalfSize}, {0, 1, 0}, {1, 0}},
            {{floorHalfSize, 0, floorHalfSize}, {0, 1, 0}, {1, 1}}
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
        wagl::gl::Texture albedo = wagl::loadTexture("resources/textures/soil/Soil_2x2_1K_albedo.dds");
        wagl::gl::Texture normal = wagl::loadTexture("resources/textures/soil/Soil_2x2_1K_normal.dds");
        wagl::gl::Texture roughness = wagl::loadTexture("resources/textures/soil/Soil_2x2_1K_roughness.dds");
    } soilTextures {};

    struct {
        wagl::gl::Texture albedo = wagl::loadTexture("resources/textures/decals/grass/Debris_DriedGrass01_albedo.dds");
        wagl::gl::Texture alpha = wagl::loadTexture("resources/textures/decals/grass/Debris_DriedGrass01_alpha.dds");
        wagl::gl::Texture height = wagl::loadTexture("resources/textures/decals/grass/Debris_DriedGrass01_height.dds");
        wagl::gl::Texture normal = wagl::loadTexture("resources/textures/decals/grass/Debris_DriedGrass01_normal.dds");
        wagl::gl::Texture roughness = wagl::loadTexture("resources/textures/decals/grass/Debris_DriedGrass01_roughness.dds");
    } decalTextures;

    wagl::Mesh decalsMesh;
    {
        std::uniform_real_distribution<float> positionDistribution(-floorHalfSize, floorHalfSize);
        std::uniform_real_distribution<float> sizeDistribution(0.5, 4);
        std::uniform_real_distribution<float> rotationDistribution(0, 2 * glm::pi<float>());
        for (int i = 0; i < 1000; i++) {
            glm::vec2 pos{
                    positionDistribution(randomGenerator),
                    positionDistribution(randomGenerator)
            };

            float a = rotationDistribution(randomGenerator);
            float s = sizeDistribution(randomGenerator);

            glm::vec2 pa = glm::rotate(glm::vec2(-s), a) + pos;
            glm::vec2 pb = glm::rotate(glm::vec2(s), a) + pos;

            wagl::Mesh::Vertex vertices[4];
            vertices[0].position = {glm::rotate(glm::vec2(-s, -s), a) + pos, 0.0f};
            vertices[1].position = {glm::rotate(glm::vec2(-s,  s), a) + pos, 0.0f};
            vertices[2].position = {glm::rotate(glm::vec2( s, -s), a) + pos, 0.0f};
            vertices[3].position = {glm::rotate(glm::vec2( s,  s), a) + pos, 0.0f};
            vertices[0].normal = vertices[1].normal = vertices[2].normal = vertices[3].normal = {0,1,0};
            vertices[0].texcoord = {0, 0};
            vertices[1].texcoord = {0, 1};
            vertices[2].texcoord = {1, 0};
            vertices[3].texcoord = {1, 1};


            for(int j = 0; j < 4; j++) {
                decalsMesh.data.vertices.push_back(vertices[j]);
            }

            size_t indices[] = {0, 1, 2, 2, 1, 3};


            for(auto e : indices) {
                decalsMesh.data.elements.push_back(e + (i * 4));
            }
        }
        decalsMesh.update();
    }

    wagl::UniformBuffer<GenTexture> genTex;
    genTex->albedo = soilTextures.albedo.getHandle();
    genTex->albedo.makeResident();
    genTex->roughness = soilTextures.roughness.getHandle();
    genTex->roughness.makeResident();
    genTex->normal = soilTextures.normal.getHandle();
    genTex->normal.makeResident();

    wagl::UniformBuffer<DecalTextures> decalTexBuffer;
    decalTexBuffer->albedo = decalTextures.albedo.getHandle();
    decalTexBuffer->albedo.makeResident();
    decalTexBuffer->alpha = decalTextures.alpha.getHandle();
    decalTexBuffer->alpha.makeResident();
    decalTexBuffer->roughness = decalTextures.roughness.getHandle();
    decalTexBuffer->roughness.makeResident();
    decalTexBuffer->normal = decalTextures.normal.getHandle();
    decalTexBuffer->normal.makeResident();

    wagl::UniformBuffer<DecalGenInfo> decalGenInfo;

    wagl::gl::Shader terrainShader {
            {GL_VERTEX_SHADER, wagl::loadFileGLSL("resources/shaders/shader.vert",  "resources/shaders")},
            {GL_FRAGMENT_SHADER, wagl::loadFileGLSL("resources/shaders/terrain.frag", "resources/shaders")}
    };

    wagl::gl::Shader pipeGenShd {
            wagl::shd::vs_fullscreen,
            {GL_FRAGMENT_SHADER, wagl::loadFileGLSL("resources/shaders/terrainGen.frag", "resources/shaders")}
    };

    wagl::gl::Shader texToBuffer {
            {GL_COMPUTE_SHADER, wagl::loadFileGLSL("resources/shaders/texToBuffer.glsl", "resources/shaders")}
    };

    wagl::gl::Shader decalShader {
            {GL_VERTEX_SHADER, wagl::loadFileGLSL("resources/shaders/decalGen.vert", "resources/shaders")},
            {GL_FRAGMENT_SHADER, wagl::loadFileGLSL("resources/shaders/decalGen.frag", "resources/shaders")}
    };

    glm::uvec2 pageSize = wagl::gl::getFormatPageSize(GL_TEXTURE_2D, GL_R11F_G11F_B10F);
    pageSize = glm::max(pageSize, glm::uvec2( wagl::gl::getFormatPageSize(GL_TEXTURE_2D, GL_RGBA8)));
    std::cout << "pagesize " << pageSize.x << " " << pageSize.y << std::endl;

    glm::uvec2 texSize = {2048 * 8, 2048 * 8};
    int texLevels = 0;
    while(texSize.x / (1u << texLevels) >= pageSize.x) texLevels++;
    std::cout << "levels" << texLevels << std::endl;
    GLint maxTexSize;
    glGetIntegerv(GL_MAX_SPARSE_TEXTURE_SIZE_ARB, &maxTexSize);
    std::cout << "max tex size " << maxTexSize << std::endl;

    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxTexSize);
    std::cout << "max layers " << maxTexSize << std::endl;

    wagl::gl::Texture albedo(GL_TEXTURE_2D);
    albedo.storage2DSparse(texSize, texLevels, GL_RGB10);
    wagl::gl::Texture metalRoughNormal(GL_TEXTURE_2D);
    metalRoughNormal.storage2DSparse(texSize, texLevels, GL_RGBA8);


    wagl::gl::Texture commitment(GL_TEXTURE_2D);
    commitment.storage2D(texSize / pageSize, 1, GL_R32F);

    struct {
        wagl::gl::Texture albedo = wagl::gl::Texture(GL_TEXTURE_2D);
        wagl::gl::Texture roughMetalNormal = wagl::gl::Texture(GL_TEXTURE_2D);
    } stagingTextures;
    stagingTextures.albedo.storage2D(pageSize, 1, GL_RGB10);
    stagingTextures.roughMetalNormal.storage2D(pageSize, 1, GL_RGBA8);
    stagingTextures.albedo.getHandle().makeResident();
    stagingTextures.roughMetalNormal.getHandle().makeResident();

    struct {
        wagl::gl::Buffer albedo = wagl::gl::Buffer();
        wagl::gl::Buffer roughMetalNotmal = wagl::gl::Buffer();
    } pbos;
    pbos.albedo.storage(pageSize.x * pageSize.y * 4 * 4, nullptr, 0);
    pbos.roughMetalNotmal.storage(pageSize.x * pageSize.y * 4 * 4, nullptr, 0);


    wagl::gl::FrameBuffer genFrameBuffer;
    genFrameBuffer.texture(GL_COLOR_ATTACHMENT0, stagingTextures.albedo, 0);
    genFrameBuffer.texture(GL_COLOR_ATTACHMENT1, stagingTextures.roughMetalNormal, 0);
    genFrameBuffer.drawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1});

    Quadtree q;
    std::vector<uint64_t> genBatch;
    std::vector<uint64_t> compressBatch;
    std::vector<uint64_t> copyBatch;
    std::vector<uint64_t> batchRemove;

    auto commitPage = [&](uint64_t node) {
        q.add(node);
        auto pageCoord = getPageCoord(node);
        auto texelCoord = pageCoord * pageSize;
        auto level = texLevels - Quadtree::getNodeDepth(node) - 1;
        albedo.pageCommitment(level, {texelCoord, 0}, {pageSize, 1}, true);
        metalRoughNormal.pageCommitment(level, {texelCoord, 0}, {pageSize, 1}, true);

        commitment.subClear(0, {pageCoord * (1u << level), 0} , {glm::uvec2(1u << level), 1}, (float)level);
    };

    auto uncommitPage = [&](uint64_t node) {
        q.remove(node);
        auto pageCoord = getPageCoord(node);
        auto texelCoord = pageCoord * pageSize;
        auto level = texLevels - Quadtree::getNodeDepth(node) - 1;
        albedo.pageCommitment(level, {texelCoord, 0}, {pageSize, 1}, false);
        metalRoughNormal.pageCommitment(level, {texelCoord, 0}, {pageSize, 1}, false);

        commitment.subClear(0, {pageCoord * (1u << level), 0} , {glm::uvec2(1u << level), 1}, (float)level + 1);
    };

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
    terrainTexturesBuffer->levels = texLevels;

    wagl::UniformBuffer<TexToBufferInfo> texToBufferInfoBuffer;


    {
        auto uv = getPageUv(0b0111);
        wagl::rect pageWorldCoords = (uv * 2.0f - 1.0f) * floorHalfSize;

        std::cout << pageWorldCoords.min.x << " " << pageWorldCoords.min.y << " " << pageWorldCoords.max.x << " " << pageWorldCoords.max.y << std::endl;
    }


    app.run([&](float dt) {
        cameraController.updateCamera(dt);

        //Generate pages
        pipeGenShd.use();
        floor.gl.vertexArray.bind();
        wagl::UniformBuffer<GenerationInfo> genInfoBuffer;

        float distMultiplier = 1.5;

        //pages to remove
        if (app.window.getKey(GLFW_KEY_R)) {
            q.traverseNodes([&](uint64_t node) {
                batchRemove.push_back(node);
            });
            compressBatch.clear();
            copyBatch.clear();
            genBatch.clear();
        } else {
            q.traverseLeafNodes([&](uint64_t node) {
                auto level = texLevels - Quadtree::getNodeDepth(node) - 1;
                auto uvs = getPageUv(node);
                auto pageWorldCoords = (uvs * 2.0f - 1.0f) * floorHalfSize;
                auto d = (1 << (texLevels - Quadtree::getNodeDepth(node))) * distMultiplier;
                if (glm::distance(pageWorldCoords.center(), glm::vec2(camera.transform.position.x, camera.transform.position.z)) > d) {
                    batchRemove.push_back(node);
                }
            });
        }

        std::sort(batchRemove.begin(), batchRemove.end(), [](uint64_t a, uint64_t b) {
            return a > b;
        });

        for(auto p : batchRemove) {
            uncommitPage(p);
        }
        batchRemove.clear();

        //pages to copy
        for(auto p : copyBatch) {
            commitPage(p);
            auto level = texLevels - Quadtree::getNodeDepth(p) - 1;
            auto coord = getPageCoord(p);

            pbos.albedo.bind(GL_PIXEL_UNPACK_BUFFER);
            albedo.subimage2D(level, coord.x * pageSize.x, coord.y * pageSize.y, pageSize.x, pageSize.y, GL_RGBA, GL_FLOAT, nullptr);

            pbos.roughMetalNotmal.bind(GL_PIXEL_UNPACK_BUFFER);
            metalRoughNormal.subimage2D(level, coord.x * pageSize.x, coord.y * pageSize.y, pageSize.x, pageSize.y, GL_RGBA, GL_FLOAT, nullptr);

            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }

        copyBatch.clear();

        for(auto p : compressBatch) {
            texToBuffer.use();
            pbos.albedo.bindRange(GL_SHADER_STORAGE_BUFFER, 1, 0, pbos.albedo.size);

            texToBufferInfoBuffer.invalidate();
            texToBufferInfoBuffer->tex = stagingTextures.albedo.getHandle();
            texToBufferInfoBuffer->lod = 0;
            texToBufferInfoBuffer.bind(0);
            glDispatchCompute(pageSize.x, pageSize.y, 1);

            pbos.roughMetalNotmal.bindRange(GL_SHADER_STORAGE_BUFFER, 1, 0, pbos.roughMetalNotmal.size);
            texToBufferInfoBuffer.invalidate();
            texToBufferInfoBuffer->tex = stagingTextures.roughMetalNormal.getHandle();
            texToBufferInfoBuffer->lod = 0;
            texToBufferInfoBuffer.bind(0);
            glDispatchCompute(pageSize.x, pageSize.y, 1);

            copyBatch.push_back(p);
        }

        compressBatch.clear();

        //pages to generate
        q.traversePotentialNodes([&](uint64_t node) {
            auto level = texLevels - Quadtree::getNodeDepth(node) - 1;

            if (level >= 0) {
                auto uvs = getPageUv(node);
                auto pageWorldCoords = (uvs * 2.0f - 1.0f) * floorHalfSize;
                /*if (pageWorldCoords.contains(glm::vec2(camera.transform.position.x, camera.transform.position.z))) {
                    batchAdd.push_back(node);
                }*/
                auto d = (1 << (texLevels - Quadtree::getNodeDepth(node))) * distMultiplier;
                auto distance = glm::distance(pageWorldCoords.center(), glm::vec2(camera.transform.position.x, camera.transform.position.z));
                //std::cout << distance << " " << d << std::endl;
                if (distance < d) {
                    genBatch.push_back(node);
                }
            }
        });

        std::sort(genBatch.begin(), genBatch.end(), [](uint64_t a, uint64_t b){
            return a < b;
        });
        for(auto p : genBatch) {
            uint64_t level = texLevels - Quadtree::getNodeDepth(p) - 1;
            auto coord = getPageCoord(p);

            auto uvs = getPageUv(p);
            auto pageWorldCoords = (uvs * 2.0f - 1.0f) * floorHalfSize;

            //Generate soil
            genFrameBuffer.bind();
            genInfoBuffer.invalidate();

            genInfoBuffer->lod = level;
            genInfoBuffer->offset = coord;
            genInfoBuffer->size = glm::vec2(1);
            genInfoBuffer.bind(1);
            genTex.bind(0);

            glViewport(0, 0, pageSize.x, pageSize.y);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            //decals!
            decalGenInfo.invalidate();
            decalGenInfo->proj = glm::ortho(pageWorldCoords.min.x, pageWorldCoords.max.x, pageWorldCoords.min.y, pageWorldCoords.max.y);
            decalGenInfo.bind(0);
            decalTexBuffer.bind(1);
            decalShader.use();
            decalsMesh.gl.vertexArray.bind();
            glDrawElements(GL_TRIANGLES, decalsMesh.gl.count, GL_UNSIGNED_SHORT, nullptr);

            compressBatch.push_back(p);
            break; //only do one per frame for now.
        }

        genBatch.clear();


        wagl::gl::FrameBuffer::unbind();

        //commitment.subClear(0, {0,0, 0}, {texSize/pageSize, 1}, 4.0f);

        //Render frame


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