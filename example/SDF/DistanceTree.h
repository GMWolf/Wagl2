//
// Created by felix on 5/11/2019.
//

#ifndef WAGL2_DISTANCETREE_H
#define WAGL2_DISTANCETREE_H

#include <iostream>
#include <vector>
#include "SDF.h"

struct DistanceTree {

    struct Node {
        float dist;
        size_t children[8];
    };

    std::vector<Node> nodes;

    DistanceTree() {
        nodes.emplace_back();
    }

    float get(glm::vec3 pos, glm::vec3 tpos, float dim, int idx) {
        glm::bvec3 g = glm::greaterThan(pos, tpos);
        int i = (g.x ? 1 :0) + (g.y ? 2 : 0) + (g.z ? 4 : 0);
        if (nodes[idx].children[i] != 0) {
            glm::vec3 cpos;
            float hdim = dim / 2;
            cpos.x = tpos.x + ((i & 1) ?  hdim : -hdim);
            cpos.y = tpos.y + ((i & 2) ?  hdim : -hdim);
            cpos.z = tpos.z + ((i & 4) ?  hdim : -hdim);
            return get(pos, cpos, hdim, nodes[idx].children[i]);
        } else {
            return nodes[idx].dist;
        }
    }

    void compute(const wagl::Mesh::MeshData& md, glm::vec3 pos, float dim, size_t nodeIndex, int maxDepth)
    {
        nodes[nodeIndex].dist = sdfMeshData(pos, md);

        if ((maxDepth > 0) && (nodes[nodeIndex].dist < sqrtf(3.0f) * dim)) {
            for(int i = 0; i < 8; i++) {
                nodes[nodes[nodeIndex].children[i]].dist = sdfMeshData(pos, md);

                nodes[nodeIndex].children[i] = nodes.size();
                nodes.emplace_back();
                glm::vec3 cpos;
                float hdim = dim / 2;
                cpos.x = pos.x + ((i & 1) ?  hdim : -hdim);
                cpos.y = pos.y + ((i & 2) ?  hdim : -hdim);
                cpos.z = pos.z + ((i & 4) ?  hdim : -hdim);
                compute(md, cpos, hdim, nodes[nodeIndex].children[i], maxDepth - 1);
            }
        } else {
            for(int i = 0; i < 8; i++) {
                nodes[nodeIndex].children[i] = 0;
            }
        }
    }

};

#endif //WAGL2_DISTANCETREE_H
