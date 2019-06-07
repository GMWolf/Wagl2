//
// Created by felix on 5/5/2019.
//

#ifndef WAGL2_MODEL_H
#define WAGL2_MODEL_H

#include <vector>
#include "gl/Buffer.h"
#include "gl/VertexArray.h"
#include <glm/glm.hpp>
#include "util/Utils.h"

namespace wagl {

    class Mesh {
    public:
        struct Vertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texcoord;
        };

        struct MeshData {
            std::vector<Vertex> vertices;
            std::vector<GLushort> elements;

            util::block getBlock() const;
        } data;

        struct MeshGL{
            MeshGL();
            explicit MeshGL(const MeshData& md);

            gl::Buffer buffer;
            gl::VertexArray vertexArray;

            size_t first;
            size_t count;
            GLenum elementType;

            void updateData(const MeshData& md);

        private:
            void setupAttributes();
            void setupBindings(size_t vertexOffset);
        } gl;


        Mesh();
        explicit Mesh(MeshData);
        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(Mesh&&) noexcept = default;

        void update();
    };
}


#endif //WAGL2_MODEL_H
