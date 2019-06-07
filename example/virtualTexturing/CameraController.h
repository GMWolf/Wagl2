//
// Created by felix on 07/06/2019.
//

#ifndef WAGL2_CAMERACONTROLLER_H
#define WAGL2_CAMERACONTROLLER_H

#include <Window.h>
#include <Camera.h>

struct CameraController {

    void updateCamera(float dt);

    wagl::Window& window;
    wagl::Camera& camera;
    std::optional<glm::vec2> dragStart = {};

    float moveSpeed = 30;
    float runMultiplier = 2;
    float floor = camera.transform.position.y;
    float vspeed = 0;
};


#endif //WAGL2_CAMERACONTROLLER_H
