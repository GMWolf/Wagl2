//
// Created by felix on 13/06/2019.
//

#ifndef WAGL2_UI_H
#define WAGL2_UI_H

#include "../Geom.h"
#include "../InputProcessor.h"

namespace wagl::ui {

    struct UIContext {
    };

    class Component : public DefaultInputProcessor {
    public:
        virtual void updateBounds(rect newRect) = 0;
        virtual void render(UIContext& ctx) = 0;
        virtual ~Component() = default;
        rect bounds;
    };
}

#endif //WAGL2_UI_H
