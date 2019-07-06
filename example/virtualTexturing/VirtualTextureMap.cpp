//
// Created by felix on 09/06/2019.
//

#include "VirtualTextureMap.h"
#include <iostream>

VirtualTextureMap::VirtualTextureMap(glm::uvec2 _size) :
    size(_size),
    commitment(GL_TEXTURE_2D),
    albedo(GL_TEXTURE_2D),
    metalRoughnessNormal(GL_TEXTURE_2D) {

    glm::ivec3 pageSize3 = wagl::gl::getFormatPageSize(GL_TEXTURE_2D, GL_R11F_G11F_B10F);
    pageSize3 = glm::max(pageSize3, wagl::gl::getFormatPageSize(GL_TEXTURE_2D, GL_RGBA8));
    pageSize = glm::uvec2(pageSize3);

    //compute levels
    levels = 1;
    while(glm::any(glm::greaterThan(size / glm::uvec2(glm::pow(2u, levels)), pageSize))) levels++;
    //levels = log2(size / pageSize)

    albedo.storage2DSparse(size, levels, GL_R11F_G11F_B10F);
    metalRoughnessNormal.storage2DSparse(size, levels, GL_RGBA8);
    commitment.storage2D(size / pageSize, 1, GL_R8);
    //compute possible levels
    std::cout << "levels " << levels << std::endl;
    glm::uvec2 baseMipSize = size / glm::uvec2(glm::pow(2, levels));

    glm::uvec2 basePageCount = (baseMipSize + (pageSize - 1u)) / pageSize;
    std::cout << "base pages " << basePageCount.x << " " << basePageCount.y << std::endl;

    //create base pages
    basePage.children = {};
    basePage.level = (levels - 1);
    basePage.committed = false;
}

