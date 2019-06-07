//
// Created by felix on 04/06/2019.
//

#ifndef WAGL2_INPUTPROCESSOR_H
#define WAGL2_INPUTPROCESSOR_H

#include <glm/glm.hpp>
#include <vector>

namespace wagl {

    enum class MouseButton {
        LEFT,
        RIGHT,
        MIDDLE
    };

    class InputProcessor {
    public:
        virtual bool mouseButtonPress(MouseButton button, glm::vec2 mousePos) = 0;
        virtual bool mouseButtonRelease(MouseButton button, glm::vec2 mousePos) = 0;
    };

    class DefaultInputProcessor : public InputProcessor {
    public:
        bool mouseButtonPress(MouseButton button, glm::vec2 mousePos) override {
            return false;
        };
        bool mouseButtonRelease(MouseButton button, glm::vec2 mousePos) override  {
            return false;
        }
    };

    class InputMultiplexer : public InputProcessor {
    public:
        bool mouseButtonPress(MouseButton button, glm::vec2 mousePos) override {
            for(InputProcessor* processor : processors) {
                if (processor->mouseButtonPress(button, mousePos)) {
                    return true;
                }
            }
            return false;
        }

        bool mouseButtonRelease(MouseButton button, glm::vec2 mousePos) override {
            for(InputProcessor* processor : processors) {
                if (processor->mouseButtonRelease(button, mousePos)) {
                    return true;
                }
            }
            return false;
        }

    private:
        std::vector<InputProcessor*> processors;
    };
}

#endif //WAGL2_INPUTPROCESSOR_H
