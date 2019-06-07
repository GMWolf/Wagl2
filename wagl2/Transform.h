//
// Created by felix on 5/4/2019.
//

#ifndef WAGL2_TRANSFORM_H
#define WAGL2_TRANSFORM_H

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

namespace wagl {
    struct Transform {
        glm::vec3 position = glm::vec3(0,0,0);
        float scale = 1;
        glm::quat orientation = glm::quat();

        void lookat(glm::vec3 target, glm::vec3 up = glm::vec3(0, 1, 0));

        glm::vec3 forward() const;

        glm::vec3 right() const;

        glm::vec3 up() const;

        glm::vec3 apply(glm::vec3 p) const;

        glm::mat4 matrix() const;
    };
}

#endif //WAGL2_TRANSFORM_H
