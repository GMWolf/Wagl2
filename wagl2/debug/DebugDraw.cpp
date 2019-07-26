//
// Created by felix on 25/07/2019.
//

#include "DebugDraw.h"
#include "DebugShaders.h"

wagl::debugdraw::DebugDrawContext::DebugDrawContext(uint64_t batchSize) :
shader({shd::vs_debug, shd::fs_debug})
{
    vertexBuffer.storage(batchSize * sizeof(vertex3d), nullptr, GL_DYNAMIC_STORAGE_BIT);

    vertexArray.vertexBuffer(0, vertexBuffer, 0, sizeof(vertex3d));
    vertexArray.attribFormat(0, 3, GL_FLOAT, false, offsetof(vertex3d, pos));
    vertexArray.attribFormat(1, 3, GL_UNSIGNED_BYTE, true, offsetof(vertex3d, color));
    vertexArray.attribBinding(0, 0);
    vertexArray.attribBinding(1, 0);
    vertexArray.enableAttrib(0);
    vertexArray.enableAttrib(1);
}


void wagl::debugdraw::DebugDrawContext::addLine(glm::vec3 a, glm::vec3 b, glm::u8vec3 color) {
    lineVertices.push_back({a, color});
    lineVertices.push_back({b, color});
}

void wagl::debugdraw::DebugDrawContext::draw(glm::mat4 viewProjection) {

    vertexBuffer.invalidateData();
    vertexBuffer.bufferData(lineVertices);


    projBuffer.invalidate();
    *projBuffer = viewProjection;
    projBuffer.bind(0);
    vertexArray.bind();
    shader.use();
    glDrawArrays(GL_LINES, 0, lineVertices.size());

    lineVertices.clear();
}


