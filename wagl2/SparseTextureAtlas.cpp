//
// Created by felix on 06/06/2019.
//

#include <stdexcept>
#include <iostream>
#include "SparseTextureAtlas.h"


static size_t orderOf(size_t n) {
    int p = 1;
    int o = 0;
    while(p < n) {
        p *= 2;
        o++;
    }
    return o;
}

static glm::uvec2 orderOf(glm::uvec2 n) {
    return glm::uvec2(orderOf(n.x), orderOf(n.y));
}

wagl::gl::TextureHandle wagl::SparseTextureAtlas::gethandle(const wagl::gl::Sampler &sampler) {
    return texture.getHandle(sampler);
}

wagl::SparseTextureAtlas::SparseTextureAtlas(glm::uvec2 _virtualSize, size_t levels, GLenum format) :
        texture(GL_TEXTURE_2D),
        freelists(0,0),
        levels(levels)
{

    glm::uvec2 order  = orderOf(_virtualSize);
    virtualSize = glm::exp2(glm::vec2(order));

    GLint pageWidth, pageHeight;
    glGetInternalformativ( GL_TEXTURE_2D, format, GL_VIRTUAL_PAGE_SIZE_X_ARB, 1, &pageWidth);
    glGetInternalformativ( GL_TEXTURE_2D, format, GL_VIRTUAL_PAGE_SIZE_Y_ARB, 1, &pageHeight);
    pageSize = {pageWidth, pageHeight};
    std::cout << "page size " << pageWidth << " " << pageHeight << "\n";

    pageOrder = orderOf(pageSize);
    maxOrder = order - pageOrder;

    texture.storage2DSparse(virtualSize.x, virtualSize.y, levels, format);

    //populate freelist
    freelists.realloc(maxOrder.x + 1, maxOrder.y + 1);
    freelists(maxOrder.x, maxOrder.y).push_back({
        {0,0},
        {virtualSize.x, virtualSize.y}
    });

}

static wagl::urect lowerX(wagl::urect b) {
    return {
            b.min,
            {b.min.x + (b.max.x - b.min.x) / 2, b.max.y}
    };
}

static wagl::urect upperX(wagl::urect b) {
    return {
            {b.min.x + (b.max.x - b.min.x) / 2, b.min.y},
            b.max
    };
}

static wagl::urect lowerY(wagl::urect b) {
    return {
            b.min,
            {b.max.x, b.min.y + (b.max.y - b.min.y) / 2}
    };
}

static wagl::urect upperY(wagl::urect b) {
    return {
            {b.min.x, b.min.y + (b.max.y - b.min.y) / 2},
            b.max
    };
}

wagl::urect wagl::SparseTextureAtlas::allocOrder(glm::uvec2 order) {
    if (!freelists(order.x, order.y).empty()) {
        urect ret = freelists(order.x, order.y).back();
        freelists(order.x, order.y).pop_back();
        return ret;
    }

    //TODO find a better iteration for better usage
    for(int ox = order.x; ox <= maxOrder.x; ox++){
        for(int oy = order.y; oy <= maxOrder.y; oy++) {
            if (!freelists(ox, oy).empty()) {
                urect block = freelists(ox, oy).back();
                freelists(ox, oy).pop_back();

                //subdiv it along the x axis;
                while(ox > order.x) {
                    ox--;
                    urect low = lowerX(block);
                    urect up = upperX(block);
                    freelists(ox, oy).push_back(up);
                    block = low;
                }

                //subdiv in along y axis
                while(oy > order.y) {
                    oy--;
                    urect low = lowerY(block);
                    urect up = upperY(block);
                    freelists(ox, oy).push_back(up);
                    block = low;
                }
                return block;
            }
        }
    }

    throw std::runtime_error("Could not allocate");
}

wagl::urect wagl::SparseTextureAtlas::alloc(glm::uvec2 size) {
    glm::uvec2 order = orderOf(size);
    order = glm::min(glm::uvec2(0), order);
    urect block = allocOrder(order);

    for(int i = 0; i < levels; i++) {
        texture.pageCommitment(i, block.min.x, block.min.y, 0, block.size().x, block.size().y, 1, true);
    }
    block.max= block.min + size; //make it the size we want
    return block;
}

