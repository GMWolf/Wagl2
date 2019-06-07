//
// Created by felix on 04/06/2019.
//

#ifndef WAGL2_APPLICATION_H
#define WAGL2_APPLICATION_H

#include "Window.h"
#include "InputProcessor.h"
#include <string>

namespace wagl {

    struct AppConfig {
        int width;
        int height;
        std::string title;
    };

    class Application {
    public:
        explicit Application(const AppConfig& config);

        InputMultiplexer inputMultiplexer;
    private:
        Window window;

        static constexpr MouseButton mouseButtons[] {
            MouseButton::LEFT,
            MouseButton::RIGHT,
            MouseButton::MIDDLE
        };
    };

}


#endif //WAGL2_APPLICATION_H
