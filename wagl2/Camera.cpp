//
// Created by felix on 5/4/2019.
//

#include "Camera.h"

glm::mat4 wagl::Camera::viewProjection(float aspect) const {
    return projection(aspect) * glm::inverse(transform.matrix());
}

glm::mat4 wagl::Camera::view() const {
    return glm::inverse(transform.matrix());
}

glm::mat4 wagl::Camera::projection(float aspect) const {
    return glm::perspective(fovy, aspect, znear, zfar);
}
