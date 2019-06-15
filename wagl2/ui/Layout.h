//
// Created by felix on 04/06/2019.
//

#ifndef WAGL2_LAYOUT_H
#define WAGL2_LAYOUT_H

#include "UI.h"
#include <memory>

namespace wagl::ui {

    class SplitLayout : public Component {
    public:
        std::unique_ptr<Component> child[2];

        void updateBounds(rect newRect) override;

        void render(UIContext &ctx) override;

        bool mouseButtonPress(MouseButton button, glm::vec2 mousePos) override;

    private:
        enum class SplitDir {
            Horizontal,
            Vertical
        } splitDir = SplitDir::Horizontal;

        float splitPos = 0.5;
        bool relativeSplitPos = true;
    };

}

#endif //WAGL2_LAYOUT_H
