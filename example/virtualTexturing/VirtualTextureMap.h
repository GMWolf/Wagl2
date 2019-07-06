//
// Created by felix on 09/06/2019.
//

#ifndef WAGL2_VIRTUALTEXTUREMAP_H
#define WAGL2_VIRTUALTEXTUREMAP_H
#include <gl/Texture.h>
#include <glm/glm.hpp>
#include <util/Matrix.h>
#include <queue>
#include <memory>
#include <array>

class VirtualTextureMap {
public:

    wagl::gl::Texture albedo;
    wagl::gl::Texture metalRoughnessNormal;

    /* 0 1 *
     * 2 2 */

    struct Page {
        size_t level; //mip level
        glm::uvec2 imageCoord; //coordinates on image
        bool committed;
        bool valid;

        std::unique_ptr<Page[]> children {};
    };

    glm::uvec2 size;
    glm::uvec2 pageSize;
    int levels;

    wagl::gl::Texture commitment;

    explicit VirtualTextureMap(glm::uvec2 size);

    template<class L>
    void traversePages(L fun);

    void subdiv(Page& page) {
        assert(page.level > 0);
        page.children = std::make_unique<Page[]>(4);
        for(unsigned int i = 0; i < 4; i++) {
            page.children[i].level = page.level - 1;
            page.children[i].committed = false;
            page.children[i].valid = false;
        }
    }

    void commitPage(Page& page);

    std::deque<Page*> commitmentQueue;
    Page basePage;
private:
    template<class L>
    void traversePages(Page& page, L fun);
};


template<class L>
void VirtualTextureMap::traversePages(VirtualTextureMap::Page &page, L fun) {
    fun(page);
    for(int ic = 0; ic < 4; ic++) {
        if (page.children) {
            for(int i = 0; i < 4; i++) {
                traverseNode(page.children[i], fun);
            }
        }
    }
}

template<class L>
void VirtualTextureMap::traversePages(L fun) {
    traverseNode(basePage, fun);
}


#endif //WAGL2_VIRTUALTEXTUREMAP_H
