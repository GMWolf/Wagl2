//
// Created by felix on 06/07/2019.
//

#ifndef WAGL2_QUADTREE_H
#define WAGL2_QUADTREE_H

#include <bitset>
#include <unordered_set>
#include <vector>
#include "clz.h"

/**
 * An incomplete, empty quad tree implementation
 */

class Quadtree {
public:

    static int getNodeDepth(uint64_t idx);
    static uint64_t getParentNodeCode(uint64_t idx);
    static uint64_t getChildNodeCode(uint64_t idx, uint64_t childIndex);
    static uint64_t rootNodeCode();
    static uint64_t getLevelCode(uint64_t code, uint64_t depth);
    static bool isChildIndex(uint64_t index);

    void add(uint64_t nodeIndex);
    void remove(uint64_t nodeINdex);
    bool nodeHasChild(uint64_t node, uint64_t childIndex);
    bool nodeHasChild(uint64_t node);

    template<class Fun>
    void traverseNodes(Fun fun, uint64_t nodeCode);

    template<class Fun>
    void traverseNodes(Fun fun) {
        if (!nodes.empty()) {
            traverseNodes(fun, 0b01);
        }
    }


    template<class Fun>
    void traverseLeafNodes(Fun fun, uint64_t nodeCode);
    template<class Fun>
    void traverseLeafNodes(Fun fun) {
        if (!nodes.empty()) {
            traverseLeafNodes(fun, 0b01);
        }
    }

    template<class Fun>
    void traversePotentialNodes(Fun fun, uint64_t nodeCode);template<class Fun>
    void traversePotentialNodes(Fun fun) {
        if (!nodes.empty()) {
            traversePotentialNodes(fun, 0b01);
        } else {
            fun(0b01);
        }
    }

private:
    std::unordered_set<uint64_t> nodes;
};

void Quadtree::add(uint64_t nodeIndex) {
    auto n = nodeIndex;
    assert((nodeIndex == rootNodeCode()) || nodes.count(getParentNodeCode(nodeIndex)));
    nodes.insert(nodeIndex);
}

int Quadtree::getNodeDepth(uint64_t idx) {
    return msb_n(idx, 2);
}

uint64_t Quadtree::getParentNodeCode(uint64_t idx) {
    return idx >> 2u;
}

uint64_t Quadtree::getChildNodeCode(uint64_t idx, uint64_t childIndex) {
    assert(isChildIndex(childIndex));
    return (idx << 2u) | childIndex;
}

uint64_t Quadtree::rootNodeCode() {
    return 0b01;
}

bool Quadtree::isChildIndex(uint64_t index) {
    return (index & ~0b11u) == 0;
}

bool Quadtree::nodeHasChild(uint64_t node, uint64_t childIndex) {
    return nodes.count(getChildNodeCode(node, childIndex)) > 0;
}

bool Quadtree::nodeHasChild(uint64_t node) {
    return nodeHasChild(node, 0b00)
           || nodeHasChild(node, 0b01)
           || nodeHasChild(node, 0b10)
           || nodeHasChild(node, 0b11);
}

template<class Fun>
void Quadtree::traverseNodes(Fun fun, uint64_t nodeCode) {
    fun(nodeCode);
    for(int childId = 0; childId < 4; childId++) {
        uint64_t childCode = getChildNodeCode(nodeCode, childId);
        if (nodes.count(childCode) > 0) {
            traverseNodes(fun, childCode);
        }
    }
}

template<class Fun>
void Quadtree::traverseLeafNodes(Fun fun, uint64_t nodeCode) {
    bool hasChild = false;
    for(int childId = 0; childId < 4; childId++) {
        uint64_t childCode = getChildNodeCode(nodeCode, childId);
        if (nodes.count(childCode) > 0) {
            hasChild = true;
            traverseLeafNodes(fun, childCode);
        }
    }
    if (!hasChild) {
        fun(nodeCode);
    }
}

template<class Fun>
void Quadtree::traversePotentialNodes(Fun fun, uint64_t nodeCode) {
    for(int childId = 0; childId < 4; childId++) {
        uint64_t childCode = getChildNodeCode(nodeCode, childId);
        if (nodes.count(childCode) > 0) {
            traversePotentialNodes(fun, childCode);
        } else {
            fun(childCode);
        }
    }
}

void Quadtree::remove(uint64_t nodeIndex) {
    assert(nodes.count(nodeIndex));
    assert(!nodeHasChild(nodeIndex));
    nodes.erase(nodeIndex);
}

uint64_t Quadtree::getLevelCode(uint64_t code, uint64_t depth) {
    uint64_t nodeDepth = getNodeDepth(code);
    assert(depth <= nodeDepth);
    return code >> (2u * ((nodeDepth - depth) - 1)) & 0b11u;
}


#endif //WAGL2_QUADTREE_H
