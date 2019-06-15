//
// Created by felix on 09/06/2019.
//

#ifndef WAGL2_VIRTUALTEXTUREMAP_H
#define WAGL2_VIRTUALTEXTUREMAP_H
#include <gl/Texture.h>
#include <glm/glm.hpp>
#include <util/Matrix.h>
#include <queue>

class VirtualTextureMap {
public:
    union {
        struct {
            wagl::gl::Texture albedo;
            wagl::gl::Texture metal;
            wagl::gl::Texture roughness;
            wagl::gl::Texture normal;
            wagl::gl::Texture emissive;
        };
        wagl::gl::Texture textures[5];
    };

    GLenum formats[5] {
            GL_RGBA8,
            GL_R8,
            GL_R8,
            GL_RG8,
            GL_RGBA8
    };

    struct Page {
        size_t level;
        glm::uvec2 coord;
        bool committed;
        bool valid;
    };

    glm::uvec2 size;
    glm::uvec2 pageSize;

    wagl::gl::Texture commitment;

    VirtualTextureMap(glm::uvec2 size, size_t levels);

private:
    std::deque<Page*> commitmentQueue;

    struct Node {
        Page page;
        Node* children[4];
    };

    wagl::Matrix<Node> baseNodes;

    template<class L>
    void traverseNode(Node& node, L fun);

    template<class L>
    void traverse(L fun);

    void commitPage(Page& page);
};


template<class L>
void VirtualTextureMap::traverseNode(VirtualTextureMap::Node &node, L fun) {
    fun(node);
    for(int ic = 0; ic < 4; ic++) {
        if (node.children[ic]) {
            traverseNode(*node.children[ic], fun);
        }
    }
}

template<class L>
void VirtualTextureMap::traverse(L fun) {
    for(auto& node : baseNodes) {
        traverseNode(node, fun);
    }
}


#endif //WAGL2_VIRTUALTEXTUREMAP_H
