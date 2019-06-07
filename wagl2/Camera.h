//
// Created by felix on 5/4/2019.
//

#ifndef WAGL2_CAMERA_H
#define WAGL2_CAMERA_H

#include <glm/glm.hpp>
#include "Transform.h"

namespace wagl {

    struct Camera {
        Transform transform;
        float fovy;
        float znear;
        float zfar;

        glm::mat4 view() const;

        glm::mat4 projection(float aspect) const;

        glm::mat4 viewProjection(float aspect) const;
    };
}


#endif //WAGL2_CAMERA_H
