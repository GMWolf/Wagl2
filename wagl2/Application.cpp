//
// Created by felix on 04/06/2019.
//

#include "Application.h"

wagl::Application::Application(const wagl::AppConfig &config) :
        window(config.width, config.height, config.title.c_str())
{

    window.mouseButtonCallback = [this](Context*,int mb, int action, int mods) {
        glm::vec2 mousePos = window.getMousePos();
        switch (action) {
            case GLFW_PRESS:
                inputMultiplexer.mouseButtonPress(mb, mousePos);
                break;
            case GLFW_RELEASE:
                inputMultiplexer.mouseButtonRelease(mb, mousePos);
                break;
            default:;
        }
    };

}
