//
// Created by felix on 25/07/2019.
//

#ifndef CODE_DEBUGDRAW_H
#define CODE_DEBUGDRAW_H

#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp>
#include <gl/Buffer.h>
#include <gl/VertexArray.h>
#include <gl/Shader.h>
#include <vector>
#include <UniformBuffer.h>


namespace wagl::debugdraw {

    struct vertex3d {
        glm::vec3 pos;
        glm::u8vec3 color;
    };

    class DebugDrawContext {
    public:

        DebugDrawContext(uint64_t batchSize);

        std::vector<vertex3d> lineVertices;

        void addLine(glm::vec3 a, glm::vec3 b, glm::u8vec3 color);

        void draw(glm::mat4 viewProjection);

    private:
        gl::Buffer vertexBuffer;
        gl::VertexArray vertexArray;
        gl::Shader shader;
        UniformBuffer<glm::mat4> projBuffer;
    };

}


#endif //CODE_DEBUGDRAW_H
