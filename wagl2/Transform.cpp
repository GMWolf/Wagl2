//
// Created by felix on 5/4/2019.
//

#include "Transform.h"
#include <glm/gtx/quaternion.hpp>
#include <iostream>

glm::mat4 wagl::Transform::matrix() const{
    auto r = glm::toMat4(orientation);
    r[0][0] *= scale;
    r[1][1] *= scale;
    r[2][2] *= scale;
    r[3] = glm::vec4(position, 1.0);
    return r;
}

void wagl::Transform::lookat(glm::vec3 target, glm::vec3 up) {
    orientation = glm::quatLookAt(glm::normalize(target - position), up);
}

glm::vec3 wagl::Transform::forward() const {
    return orientation * glm::vec3(0,0,-1);
}

glm::vec3 wagl::Transform::right() const {
    return orientation * glm::vec3(1,0,0);
}

glm::vec3 wagl::Transform::up() const {
    return orientation * glm::vec3(0,1,0);
}

glm::vec3 wagl::Transform::apply(glm::vec3 p) const {
    return orientation * (p * scale) + position;
}
