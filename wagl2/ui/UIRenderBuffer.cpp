//
// Created by felix on 06/06/2019.
//

#include "UIRenderBuffer.h"

wagl::ui::UIRenderBuffer::UIRenderBuffer(size_t batchVertexSize) :
    batchSize(batchVertexSize)
{
    vertexBuffer.storage(batchSize * batchCount * sizeof(Vertex), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

    map = vertexBuffer.mapRange(0, vertexBuffer.size, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

    va.vertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
    va.attribFormat(0, 2, GL_FLOAT, false, offsetof(Vertex, pos));
    va.attribBinding(0, 0);
    va.enableAttrib(0);
    va.attribFormat(1, 2, GL_FLOAT, false, offsetof(Vertex, pos));
    va.attribBinding(1, 0);
    va.enableAttrib(1);

    for(int i = 0; i < batchCount; i++) {
        batches[i].first = i * batchSize;
        batches[i].vertexData = static_cast<Vertex *>(map) + (batches[i].first);
        batches[i].count = 0;
    }

}

void wagl::ui::UIRenderBuffer::submit() {
    vertexBuffer.flushMappedRange(batch->first * sizeof(Vertex), batch->count * sizeof(Vertex));
    va.bind();
    glDrawArrays(GL_TRIANGLES, batch->first, batch->count);
    batch->fence.sync();
    batch->count = 0;
    if(++batchIndex >= batchCount) batchIndex = 0;
    batch = &batches[batchIndex];
}

void wagl::ui::UIRenderBuffer::buffer(wagl::ui::UIRenderBuffer::Vertex *vertices, size_t count) {
    assert(batch->count + count < batchSize);
    Vertex* dest = getMapped(count);
    memcpy(dest, vertices, count);
}

wagl::ui::UIRenderBuffer::Vertex *wagl::ui::UIRenderBuffer::getMapped(size_t count) {
    assert(batch->count + count <= batchSize);
    Vertex* ptr = batch->vertexData + batch->count;
    batch->count += count;
    return ptr;
}
