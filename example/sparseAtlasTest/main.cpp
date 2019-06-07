//
// Created by felix on 06/06/2019.
//
#include <SparseTextureAtlas.h>
#include <Window.h>
#include <iostream>
#include <util/Utils.h>
#include <util/files.h>
#include <Model.h>
#include <gl/Shader.h>
#include <UniformBuffer.h>

void addTex(wagl::SparseTextureAtlas& atlas, glm::uvec2 size, glm::vec3 color) {
    auto r = atlas.alloc(size);

    std::cout << r.min.x << " " << r.min.y << " " << r.max.x << " " << r.max.y << std::endl;
    std::vector<glm::vec3> data;
    data.resize(r.size().x * r.size().y, color);

    atlas.texture.subimage2D(0, r.min.x, r.min.y, r.size().x, r.size().y, GL_RGB, GL_FLOAT, data.data());

}

void app() {

    wagl::Window window(1280, 720, "Atlas");

    wagl::SparseTextureAtlas atlas({256, 256}, 1, GL_RGB32F);

    addTex(atlas, {8,6}, {1,0,0});
    addTex(atlas, {32, 32}, {0,1,0});
    addTex(atlas, {65, 65}, {1,1,0});
    addTex(atlas, {45, 128}, {1,1,0});

    wagl::gl::Sampler sampler {
    };

    auto texHandle = atlas.gethandle(sampler);
    texHandle.makeResident();

    wagl::Mesh mesh;
    mesh.data.vertices = {
            {{-0.8,-0.8,0}, {0,0,0}, {0,0}},
            {{-0.8,0.8,0}, {0,0,0}, {0,1}},
            {{0.8,-0.8,0}, {0,0,0}, {1,0}},
            {{0.8,0.8,0}, {0,0,0}, {1,1}}
    };
    mesh.data.elements = {
            0,1,2,1,2,3
    };
    mesh.update();

    wagl::gl::Shader shd {
            {GL_VERTEX_SHADER, wagl::loadFileGLSL("resources/shaders/passthrough.vert", "resources/shaders")},
            {GL_FRAGMENT_SHADER, wagl::loadFileGLSL("resources/shaders/passthrough.frag", "resources/shaders")}
    };


    wagl::UniformBuffer<wagl::gl::TextureHandle> texBuffer;
    *texBuffer = texHandle;

    while(!window.shouldClose()) {

        shd.use();
        texBuffer.bind(2);
        mesh.gl.vertexArray.bind();
        glDrawElements(GL_TRIANGLES, mesh.gl.count, mesh.gl.elementType, reinterpret_cast<const void *>(mesh.gl.first));

        window.swapBuffers();
        wagl::pollEvents();
    }

}

int main() {

    wagl::init();

    app();

    wagl::termintate();
    return 0;
}