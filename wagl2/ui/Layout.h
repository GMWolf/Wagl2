//
// Created by felix on 04/06/2019.
//

#ifndef WAGL2_LAYOUT_H
#define WAGL2_LAYOUT_H

#include "../Geom.h"
#include "../InputProcessor.h"
#include <memory>

namespace wagl::ui {

    class Layout : public DefaultInputProcessor {
    public:
        virtual void updateBounds(rect newRect) = 0;

        rect bounds;
    };


    class SplitLayout : public Layout {
    public:
        std::unique_ptr<Layout> child[2];

        void updateBounds(rect newRect) override;

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
