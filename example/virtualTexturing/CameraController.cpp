//
// Created by felix on 07/06/2019.
//

#include "CameraController.h"

static glm::vec3 xzProject(glm::vec3 in) {
    in.y = 0;
    return glm::normalize(in);
}

void CameraController::updateCamera(float dt) {

    float speed = moveSpeed;
    if (window.getKey(GLFW_KEY_LEFT_SHIFT)) {
        speed *= runMultiplier;
    }
    if (camera.transform.position.y > floor) {
        speed *= 1.5;
    }

    glm::vec3 forward = xzProject(camera.transform.forward());
    glm::vec3 right = xzProject(camera.transform.right());
    glm::vec3 dir {0,0,0};

    if(window.getKey(GLFW_KEY_W)) {
        dir +=  forward;
    }
    if(window.getKey(GLFW_KEY_S)) {
        dir -= forward;
    }
    if(window.getKey(GLFW_KEY_D)) {
        dir += right;
    }
    if(window.getKey(GLFW_KEY_A)) {
        dir -= right;
    }

    float dl = dir.length();
    if (dl > 1.0f) {
        dir /= dl;
    }
    camera.transform.position += dir * speed * dt;

    if(window.getMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
        auto mousePos = window.getMousePos();
        if (!dragStart) {
            dragStart = mousePos;
        }

        glm::vec2 dm = mousePos - *dragStart;
        dm *= 0.001f;

        glm::quat framePitch = glm::angleAxis(-dm.y, glm::vec3(1,0,0));
        glm::quat frameYaw = glm::angleAxis(-dm.x, glm::vec3(0,1,0));

        camera.transform.orientation = frameYaw * camera.transform.orientation * framePitch ;

        window.setMousePos(*dragStart);
    } else {
        dragStart.reset();
    }

    if (camera.transform.position.y > floor) {
        vspeed -= 0.981 * dt;
    } else {
        vspeed = 0;
        camera.transform.position.y = floor;
    }



    if (camera.transform.position.y <= floor) {
        if (window.getKey(GLFW_KEY_SPACE)) {
            vspeed = 0.85;
        }
    }

    camera.transform.position.y += vspeed;

}
