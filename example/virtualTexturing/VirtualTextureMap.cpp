//
// Created by felix on 09/06/2019.
//

#include "VirtualTextureMap.h"

VirtualTextureMap::VirtualTextureMap(glm::uvec2 _size, size_t levels) :
        size(_size),
        albedo(GL_TEXTURE_2D),
        metal(GL_TEXTURE_2D),
        roughness(GL_TEXTURE_2D),
        normal(GL_TEXTURE_2D),
        emissive(GL_TEXTURE_2D),
        commitment(GL_TEXTURE_2D)
{
    albedo.storage2DSparse(size, levels, GL_RGBA8);
    metal.storage2DSparse(size, levels, GL_R8);
    roughness.storage2DSparse(size, levels, GL_R8);
    normal.storage2DSparse(size, levels, GL_RG8);
    emissive.storage2DSparse(size, levels, GL_RGBA8);

    glm::ivec3 pageSize3;
    //page size lets go
    for(int i = 0; i < 5; i++) {
        pageSize3 = glm::max(pageSize3, wagl::gl::getFormatPageSize(GL_TEXTURE_2D, formats[i]));
    }
    pageSize = glm::uvec2(pageSize3);
    glm::uvec2 basePageCount = (size + (pageSize - 1u)) / pageSize;
    size = basePageCount * pageSize;

    //create base pages
    baseNodes.realloc(basePageCount.x, basePageCount.y);
    for(int ix = 0; ix < basePageCount.x; ix++) {
        for(int iy = 0; iy < basePageCount.y; iy++) {
            for(int ic = 0; ic < 4; ic++) {
                baseNodes(ix, iy).children[ic] = nullptr;
            }

            baseNodes(ix, iy).page.level = (levels - 1);
            baseNodes(ix, iy).page.committed = false;
            baseNodes(ix, iy).page.coord = {ix, iy};
        }
    }

}

void VirtualTextureMap::commitPage(VirtualTextureMap::Page &page) {
    for(auto& tex : textures) {
        tex.pageCommitment(page.level, glm::uvec3(page.coord * pageSize, 0), glm::uvec3(pageSize, 0), true);
    }
    page.committed = true;
}


