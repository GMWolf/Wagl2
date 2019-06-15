//
// Created by felix on 06/06/2019.
//

#ifndef WAGL2_UIRENDERBUFFER_H
#define WAGL2_UIRENDERBUFFER_H

#include "gl/Buffer.h"
#include "gl/VertexArray.h"
#include "gl/Fence.h"
#include <glm/glm.hpp>

namespace wagl::ui {

    class UIRenderBuffer {
    public:
        struct Vertex {
            glm::vec2 pos;
            glm::vec2 uv;
        };
        const size_t batchSize;
        static const size_t batchCount = 3;

        explicit UIRenderBuffer(size_t batchVertexSize);

        void submit();

        void buffer(Vertex* vertices, size_t count);
        Vertex* getMapped(size_t count);

    private:
        gl::VertexArray va;
        gl::Buffer vertexBuffer;
        void* map;
        size_t batchIndex;

        struct Batch {
            Vertex* vertexData;
            gl::Fence fence;
            GLint first;
            size_t count;
        } batches [batchCount];

        Batch* batch;
    };

}
#endif //WAGL2_UIRENDERBUFFER_H
