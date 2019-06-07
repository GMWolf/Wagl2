//
// Created by felix on 04/06/2019.
//

#ifndef WAGL2_APPLICATION_H
#define WAGL2_APPLICATION_H

#include <glad/glad.h>
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

        template<class L>
        void run(L fun) {
            double lastTime = glfwGetTime();
            while(!window.shouldClose()) {
                double time = glfwGetTime();
                float dt = time - lastTime;
                lastTime = time;
                processEvents();

                fun(dt);

                window.swapBuffers();
            }
        }

        Window window;
    private:
        void processEvents() {
            wagl::pollEvents();
        }
    };

}


#endif //WAGL2_APPLICATION_H
