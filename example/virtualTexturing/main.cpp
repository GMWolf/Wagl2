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

struct PBRMaterial {
    wagl::gl::TextureHandle albedo;
    wagl::gl::TextureHandle metalRoughness;
    wagl::gl::TextureHandle emissive;
    wagl::gl::TextureHandle normal;
    wagl::gl::TextureHandle ao;
};

struct SceneInfo {
    glm::mat4 VP;
    glm::vec3 viewPos;
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
    camera.transform.position = {0, 1.75, 0};
    camera.transform.orientation = glm::quatLookAt(glm::vec3{0,0,-1}, {0,1,0});

    CameraController cameraController {app.window, camera};

    wagl::Mesh floor;
    floor.data.vertices = {
            {{-100, 0, -100}, {0, 1, 0}, {0,0}},
            {{-100, 0, 100}, {0, 1, 0}, {0, 25}},
            {{100, 0, -100}, {0, 1, 0}, {25, 0}},
            {{100, 0, 100}, {0, 1, 0}, {25, 25}}
    };
    floor.data.elements = {
            0, 1, 2, 2, 1, 3
    };
    floor.update();

    wagl::gl::Sampler sampler {
            {GL_TEXTURE_MAX_ANISOTROPY, 8}
    };

    struct {
        wagl::gl::Texture albedo = wagl::loadTexture("resources/textures/sci-fi_panel/Scifi_Panel4_1K_albedo.dds");
        wagl::gl::Texture normal = wagl::loadTexture("resources/textures/sci-fi_panel/Scifi_Panel4_1K_normal.dds");
        wagl::gl::Texture metalRoughness = wagl::loadTexture("resources/textures/sci-fi_panel/Scifi_Panel4_1K_metal_roughness.dds");
        wagl::gl::Texture ao = wagl::loadTexture("resources/textures/sci-fi_panel/Scifi_Panel4_1K_normal.dds");
        wagl::gl::Texture emissive = wagl::loadTexture("resources/textures/sci-fi_panel/Scifi_Panel4_1K_emissive.dds");
    } matTextures {};


    wagl::gl::Shader pbrShd {
            {GL_VERTEX_SHADER, wagl::loadFileGLSL("resources/shaders/shader.vert",  "resources/shaders")},
            {GL_FRAGMENT_SHADER, wagl::loadFileGLSL("resources/shaders/shader.frag", "resources/shaders")}
    };

    wagl::UniformBuffer<PBRMaterial> pbrMaterialBuffer;
    pbrMaterialBuffer->albedo = matTextures.albedo.getHandle(sampler);
    pbrMaterialBuffer->normal = matTextures.normal.getHandle(sampler);
    pbrMaterialBuffer->metalRoughness = matTextures.metalRoughness.getHandle(sampler);
    pbrMaterialBuffer->ao = matTextures.ao.getHandle(sampler);
    pbrMaterialBuffer->emissive = matTextures.emissive.getHandle(sampler);

    pbrMaterialBuffer->albedo.makeResident();
    pbrMaterialBuffer->normal.makeResident();
    pbrMaterialBuffer->metalRoughness.makeResident();
    pbrMaterialBuffer->ao.makeResident();
    pbrMaterialBuffer->emissive.makeResident();

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

        pbrShd.use();
        sceneInfoBuffer.bind(SCENE_INFO_BINDING);
        transformBlock.bind(TRANSFORM_BLOCK_BINDING);
        pbrMaterialBuffer.bind(TEXTURE_BLOCK_BINDING);


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