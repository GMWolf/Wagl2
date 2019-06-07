//
// Created by felix on 14/01/2019.
//

#include <gl/Buffer.h>
#include <gl/VertexArray.h>
#include <gl/Shader.h>
#include <gl/Draw.h>
#include <Window.h>
#include <array>
#include <iostream>
#include <util/files.h>
#include "Camera.h"
#include <glm/glm.hpp>
#include <optional>
#include <shaders/fullscreen.h>
#include <gl/Texture.h>
#include <TextureLoad.h>
#include <UniformBuffer.h>
#include <ModelLoad.h>
#include <stb_include.h>
#include <gl/FrameBuffer.h>
#include <PingPongBuffer.h>
#include <gl/Fence.h>
#include "SDF.h"
#include "DistanceTree.h"
#include "../resources/shaders/common.cshared"
#include <thread>
#include <condition_variable>
#include <queue>


void updateCamera(wagl::Window& window, wagl::Camera& camera, float dt) {
    if(window.getKey(GLFW_KEY_W)) {
        camera.transform.position += camera.transform.forward() * dt;
    }
    if(window.getKey(GLFW_KEY_S)) {
        camera.transform.position -= camera.transform.forward() * dt;
    }
    if(window.getKey(GLFW_KEY_D)) {
        camera.transform.position += camera.transform.right() * dt;
    }
    if(window.getKey(GLFW_KEY_A)) {
        camera.transform.position -= camera.transform.right() * dt;
    }

    static std::optional<glm::vec2> dragStart;

    if(window.getMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
        auto mousePos = window.getMousePos();
        if (!dragStart) {
            dragStart = mousePos;
        }

        glm::vec2 dm = mousePos - *dragStart;
        dm *= 0.001f;

        glm::quat framePitch = glm::angleAxis(-dm.y, glm::vec3(1,0,0));
        glm::quat frameYaw = glm::angleAxis(-dm.x, glm::vec3(0,1,0));

        camera.transform.orientation = frameYaw * camera.transform.orientation * framePitch ;

        window.setMousePos(*dragStart);
    } else {
        dragStart.reset();
    }
}

struct SceneInfoBlock {
    glm::mat4 viewProjection;
    glm::vec3 viewPos;
};

struct TransformBlock {
    glm::vec3 pos;
    float scale;
    glm::quat rot;
};

struct TexturesBlock {
    wagl::gl::TextureHandle albedo;
    wagl::gl::TextureHandle metalRoughness;
    wagl::gl::TextureHandle emmisive;
    wagl::gl::TextureHandle normalMap;
    wagl::gl::TextureHandle AO;
};

struct PostBlock {
    wagl::gl::TextureHandle color;
    wagl::gl::TextureHandle bloom;
};

struct BlurBlock {
    wagl::gl::TextureHandle input;
    glm::vec2 direction;
};

struct ImageBlock {
    glm::vec3 scale;
    wagl::gl::ImageHandle dest;
};

struct BBox {
    glm::vec3 min; unsigned int pad0;
    glm::vec3 max; unsigned int pad1;
};

struct VisuTexBlock {
    BBox box;
    wagl::gl::TextureHandle dist;
};


struct PbrMaterial {
    wagl::gl::TextureHandle albedo;
    wagl::gl::TextureHandle metalRoughness;
    wagl::gl::TextureHandle emmisive;
    wagl::gl::TextureHandle normalMap;
    wagl::gl::TextureHandle AO;
};

int main() {

    wagl::init();

    {
        wagl::Window window(1280, 720, "Wagl 2 application");

        if (!window.opened()) {
            std::cerr << "Failed to create context\n";
            return 1;
        }

        tinygltf::TinyGLTF tinygltf;
        tinygltf::Model gltf;
        tinygltf.LoadASCIIFromFile(&gltf, nullptr, nullptr, "resources/models/DamagedHelmet/glTF/DamagedHelmet.gltf");

        auto mesh = wagl::Mesh(wagl::loadModel(gltf, "mesh_helmet_LP_13930damagedHelmet"));

        const int sdfTexRes = 32;
        const int sdfTexLevels = 1;
        auto sdfFormat = GL_R32F;
        wagl::gl::Texture sdfTex(GL_TEXTURE_3D);
        sdfTex.parameter(GL_TEXTURE_BASE_LEVEL, 0);
        sdfTex.parameter(GL_TEXTURE_MAX_LEVEL,sdfTexLevels - 1);
        sdfTex.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        sdfTex.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        sdfTex.parameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        sdfTex.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        sdfTex.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        sdfTex.storage3D(sdfTexRes, sdfTexRes, sdfTexRes, sdfTexLevels, sdfFormat);

        wagl::util::block imageDataBlock(sizeof(float) * sdfTexRes *sdfTexRes * sdfTexRes);
        float* f = (float*)imageDataBlock.get();
        for(int i = 0; i < sdfTexRes * sdfTexRes * sdfTexRes; i++) {
            f[i + 0]     = 1;
        }

        sdfTex.submimage3D(0, 0, 0, 0, sdfTexRes, sdfTexRes, sdfTexRes, GL_RED, GL_FLOAT, imageDataBlock.get());
        wagl::gl::TextureHandle sdfTexHandle = sdfTex.getHandle();
        sdfTexHandle.makeResident();
        wagl::gl::ImageHandle sdfImage = sdfTex.getImageHandle(0, true, 0, sdfFormat);
        sdfImage.makeResident(GL_WRITE_ONLY);

        wagl::gl::Shader cs_md {
                {GL_COMPUTE_SHADER, wagl::loadFileGLSL("resources/shaders/cs_meshDistance.glsl", "shaders")}
        };

        struct {
            wagl::gl::ImageHandle dest;
        } imageBlock;
        imageBlock.dest = sdfImage;
        wagl::gl::Buffer imageBuffer(sizeof(imageBlock), &imageBlock, 0);

        struct {
            glm::vec3 min = glm::vec3(-2,-2,-2);
            int pad0;
            glm::vec3 max = glm::vec3(2,2,2);
            int pad1;
            unsigned int triangleCount;
            unsigned int pad2, pad3, pad4;
        } meshBlockHead;
        meshBlockHead.triangleCount = mesh.data.elements.size() / 3;

        wagl::util::block meshBlock(sizeof(meshBlockHead) + sizeof(glm::vec4) * mesh.data.elements.size());
        std::memcpy(meshBlock.get(), &meshBlockHead, sizeof(meshBlockHead));
        glm::vec4* positions = (glm::vec4*)(meshBlock.get() + sizeof(meshBlockHead));
        for(auto e : mesh.data.elements) {
            *(positions++) = glm::vec4(mesh.data.vertices[e].position, 0.0);
        }
        wagl::gl::Buffer meshBuffer(meshBlock, 0);

        cs_md.use();
        imageBuffer.bindRange(GL_UNIFORM_BUFFER, 0, 0, imageBuffer.size);
        meshBuffer.bindRange(GL_SHADER_STORAGE_BUFFER, 1, 0, meshBuffer.size);

        glDispatchCompute(sdfTexRes, sdfTexRes, sdfTexRes);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        sdfImage.makeNonResident();

        wagl::gl::Sampler anisotropic {
            {GL_TEXTURE_MAX_ANISOTROPY, 4.0f}
        };

        wagl::gl::Texture albedo = wagl::loadTexture("resources/models/DamagedHelmet/glTF/Default_albedo.DDS");
        wagl::gl::TextureHandle albedoHandle = albedo.getHandle(anisotropic);
        albedoHandle.makeResident();

        wagl::gl::Texture metalRoughness = wagl::loadTexture("resources/models/DamagedHelmet/glTF/Default_metalRoughness.DDS");
        wagl::gl::TextureHandle metalRoughnessHandle = metalRoughness.getHandle(anisotropic);
        metalRoughnessHandle.makeResident();

        wagl::gl::Texture emmisive = wagl::loadTexture("resources/models/DamagedHelmet/glTF/Default_emissive.DDS");
        wagl::gl::TextureHandle emmisiveHandle = emmisive.getHandle(anisotropic);
        emmisiveHandle.makeResident();

        wagl::gl::Texture AO = wagl::loadTexture("resources/models/DamagedHelmet/glTF/Default_AO.DDS");
        wagl::gl::TextureHandle AOHandle = AO.getHandle(anisotropic);
        AOHandle.makeResident();

        wagl::gl::Texture normalMap = wagl::loadTexture("resources/models/DamagedHelmet/glTF/Default_norma.DDS");
        wagl::gl::TextureHandle normalMapHandle = normalMap.getHandle(anisotropic);
        normalMapHandle.makeResident();

        wagl::gl::Texture colorTarget(GL_TEXTURE_2D);
        colorTarget.storage2D(1280, 720, 1, GL_RGB32F);
        auto colorTargetHandle = colorTarget.getHandle();
        colorTargetHandle.makeResident();
        wagl::gl::Texture brightTarget(GL_TEXTURE_2D);
        brightTarget.storage2D(1280, 720, 1, GL_RGB32F);
        brightTarget.parameter( GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        brightTarget.parameter( GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        auto brightTargetHandle = brightTarget.getHandle();
        brightTargetHandle.makeResident();

        wagl::gl::Texture depthTarget(GL_TEXTURE_2D);
        depthTarget.storage2D(1280, 720, 1, GL_DEPTH_COMPONENT24);

        wagl::gl::FrameBuffer frameBuffer;
        frameBuffer.texture(GL_COLOR_ATTACHMENT0, colorTarget, 0);
        frameBuffer.texture(GL_COLOR_ATTACHMENT1, brightTarget, 0);
        frameBuffer.texture(GL_DEPTH_ATTACHMENT, depthTarget, 0);
        frameBuffer.drawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1});

        wagl::PingPongBuffer pingpong(1280, 720, GL_RGB32F);
        pingpong.makeResident();

        wagl::gl::Shader shader{
                {GL_VERTEX_SHADER,   wagl::loadFileGLSL("resources/shaders/shader.vert", "resources/shaders")},
                {GL_FRAGMENT_SHADER, wagl::loadFileGLSL("resources/shaders/shader.frag", "resources/shaders")}
        };

        wagl::gl::Shader blurShader {
            wagl::shd::vs_fullscreen,
            {GL_FRAGMENT_SHADER, wagl::loadFileGLSL("resources/shaders/blur.frag", "resources/shaders")}
        };

        wagl::gl::Shader postShader {
            wagl::shd::vs_fullscreen,
            {GL_FRAGMENT_SHADER, wagl::loadFileGLSL("resources/shaders/bloom.frag", "resources/shaders")}
        };

        wagl::gl::Shader visShader {
            wagl::shd::vs_fullscreen,
            {GL_FRAGMENT_SHADER, wagl::loadFileGLSL("resources/shaders/fs_distanceVisu.glsl", "resources/shaders")}
        };

        wagl::Camera cam;

        cam.fovy = glm::radians(45.0f);
        cam.znear = 0.1f;
        cam.zfar = 100.f;
        cam.transform.position = glm::vec3(0, 0, -4);
        cam.transform.scale = 1.0;
        cam.transform.lookat(glm::vec3(0, 0, 0));

        wagl::UniformBuffer<SceneInfoBlock> sceneInfoBuffer;
        wagl::UniformBuffer<TexturesBlock> texturesBuffer;
        wagl::UniformBuffer<wagl::Transform> transformBuffer;
        wagl::UniformBuffer<PostBlock> postBlockBuffer;
        wagl::UniformBuffer<BlurBlock> blurBlockBuffer;
        wagl::UniformBuffer<VisuTexBlock> visuTexBuffer;

        texturesBuffer->albedo = albedoHandle;
        texturesBuffer->metalRoughness = metalRoughnessHandle;
        texturesBuffer->emmisive = emmisiveHandle;
        texturesBuffer->AO = AOHandle;
        texturesBuffer->normalMap = normalMapHandle;

        visuTexBuffer->box.min = glm::vec3(-2,-2,-2);
        visuTexBuffer->box.max = glm::vec3(2,2,2);
        visuTexBuffer->dist = sdfTexHandle;

        sceneInfoBuffer->viewProjection = cam.viewProjection(window.getAspect());

        *transformBuffer = wagl::transformFromNode(gltf.nodes[0]);

        double lastTime = glfwGetTime();

        while (!window.shouldClose()) {
            double time = glfwGetTime();
            float dt = time - lastTime;
            lastTime = time;

            updateCamera(window, cam, dt);

            glViewport(0, 0, window.getWidth(), window.getHeight());
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glDisable(GL_BLEND);

            frameBuffer.bind();
            glm::inverse(cam.transform.orientation);
            sceneInfoBuffer->viewProjection = cam.viewProjection(window.getAspect());
            sceneInfoBuffer->viewPos = cam.transform.position;

            transformBuffer->orientation = glm::angleAxis(0.5f * dt, glm::vec3(0,1,0)) *  transformBuffer->orientation;

            sceneInfoBuffer.bind(SCENE_INFO_BINDING);
            transformBuffer.bind(TRANSFORM_BLOCK_BINDING);
            texturesBuffer.bind(TEXTURE_BLOCK_BINDING);

            glClearColor(0.1, 0.1, 0.1, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            mesh.gl.vertexArray.bind();
            shader.use();
            glDrawElements(GL_TRIANGLES, mesh.gl.count, mesh.gl.elementType, nullptr);

            glGenerateTextureMipmap(brightTarget.id);

            //do blur
            glViewport(0, 0, 1280, 720);
            blurShader.use();
            blurBlockBuffer.bind(0);
            pingpong.setSource(brightTargetHandle);
            const int blurIterations = 5;
            for(int i = 0; i < blurIterations * 2; i++) {
                pingpong.bind();
                blurBlockBuffer->input = pingpong.handle();
                blurBlockBuffer->direction = (i % 2) ? glm::vec2(1,0) : glm::vec2(0, 1);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);
                blurBlockBuffer.invalidate();
                pingpong.rotate();
            }

            wagl::gl::FrameBuffer::unbind();
            glDisable(GL_BLEND);

            glViewport(0, 0, window.getWidth(), window.getHeight());
            postBlockBuffer->color = colorTargetHandle;
            postBlockBuffer->bloom = pingpong.handle();

            postShader.use();
            postBlockBuffer.bind(0);
            glDisable(GL_DEPTH_TEST);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);


            //glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            if (window.getKey(GLFW_KEY_E)) {
                visShader.use();
                sceneInfoBuffer.bind(0);
                transformBuffer.bind(1);
                visuTexBuffer.bind(2);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);
            }

            window.swapBuffers();
            wagl::pollEvents();
        }

        albedoHandle.makeNonResident();
        metalRoughnessHandle.makeNonResident();
        emmisiveHandle.makeNonResident();
        AOHandle.makeNonResident();
        normalMapHandle.makeNonResident();

        colorTargetHandle.makeNonResident();
        brightTargetHandle.makeNonResident();
        sdfTexHandle.makeNonResident();
        pingpong.makeNonResident();

    }
    wagl::termintate();

    return 0;
}