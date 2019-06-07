#include <utility>

//
// Created by felix on 5/5/2019.
//

#include "Model.h"


wagl::Mesh::Mesh() = default;

wagl::Mesh::Mesh(wagl::Mesh::MeshData md) :
    data(std::move(md)), gl(data)
{
}

void wagl::Mesh::update() {
    gl.updateData(data);
}

void wagl::Mesh::MeshGL::setupAttributes() {
    vertexArray.attribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    vertexArray.attribFormat(1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    vertexArray.attribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texcoord));
    vertexArray.attribBinding(0, 0);
    vertexArray.attribBinding(1, 0);
    vertexArray.attribBinding(2, 0);
    vertexArray.enableAttrib(0);
    vertexArray.enableAttrib(1);
    vertexArray.enableAttrib(2);
}

void wagl::Mesh::MeshGL::setupBindings(size_t vertexOffset) {
    vertexArray.vertexBuffer(0, buffer, vertexOffset, sizeof(Vertex));
    vertexArray.elementBuffer(buffer);
}

wagl::Mesh::MeshGL::MeshGL(){
    setupAttributes();
    setupBindings(0);
    first = 0;
    count = 0;
    elementType = GL_UNSIGNED_SHORT;
}

wagl::Mesh::MeshGL::MeshGL(const wagl::Mesh::MeshData &md) :
        buffer(md.getBlock(), 0)
{
    setupAttributes();
    setupBindings(util::byteSize(md.elements));
    first = 0;
    count = md.elements.size();
    elementType = GL_UNSIGNED_SHORT;
}

void wagl::Mesh::MeshGL::updateData(const MeshData &md) {
    buffer = gl::Buffer(md.getBlock(), 0);
    setupBindings(util::byteSize(md.elements));
    first = 0;
    count = md.elements.size();
    elementType = GL_UNSIGNED_SHORT;
}

wagl::util::block wagl::Mesh::MeshData::getBlock() const {
    size_t byteSize = util::byteSize(vertices) + util::byteSize(elements);
    util::block blk(byteSize);
    size_t vertexOffset = util::byteSize(elements);
    std::copy(elements.begin(), elements.end(), (GLushort*)blk.get());
    std::copy(vertices.begin(), vertices.end(), (Vertex*)(blk.get() + vertexOffset));
    return blk;
}
