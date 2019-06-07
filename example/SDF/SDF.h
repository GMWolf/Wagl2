//
// Created by felix on 5/11/2019.
//

#ifndef WAGL2_SDF_H
#define WAGL2_SDF_H

#include <glm/glm.hpp>
#include <Model.h>
#include <limits>

inline float dot2(glm::vec3 v) {
    return glm::dot(v,v);
}

inline float sdfTriangle( glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c )
{
    glm::vec3 ba = b - a; glm::vec3 pa = p - a;
    glm::vec3 cb = c - b; glm::vec3 pb = p - b;
    glm::vec3 ac = a - c; glm::vec3 pc = p - c;
    glm::vec3 nor = glm::cross( ba, ac );

    return sqrt(
            (glm::sign(glm::dot(glm::cross(ba,nor),pa)) +
             glm::sign(glm::dot(glm::cross(cb,nor),pb)) +
             glm::sign(glm::dot(glm::cross(ac,nor),pc))<2.0)
            ?
            glm::min( glm::min(
                    dot2(ba*glm::clamp(glm::dot(ba,pa)/dot2(ba),0.0f,1.0f)-pa),
                    dot2(cb*glm::clamp(glm::dot(cb,pb)/dot2(cb),0.0f,1.0f)-pb) ),
                    dot2(ac*glm::clamp(glm::dot(ac,pc)/dot2(ac),0.0f,1.0f)-pc) )
            :
            glm::dot(nor,pa)*glm::dot(nor,pa)/dot2(nor) );
}

inline float sdfMeshData(glm::vec3 p, const wagl::Mesh::MeshData& md) {

    float minDist = std::numeric_limits<float>::max();

    for(int i = 0; i < md.elements.size(); i+= 3) {
        auto a = md.vertices[md.elements[i]].position;
        auto b = md.vertices[md.elements[i+1]].position;
        auto c = md.vertices[md.elements[i+2]].position;

        float d = sdfTriangle(p, a, b, c);
        if (d < minDist) minDist = d;
    }

    return minDist;
}

#endif //WAGL2_SDF_H
