//
// Created by felix on 04/06/2019.
//

#include "Layout.h"

void wagl::ui::SplitLayout::updateBounds(wagl::rect newRect) {
    bounds = newRect;
    rect childBounds[2];
    childBounds[0].min = newRect.min;
    childBounds[1].max = newRect.max;

    if (splitDir == SplitDir::Horizontal) {
        childBounds[0].max.x = bounds.min.x + bounds.size().x * splitPos;
        childBounds[0].max.y = newRect.max.y;
        childBounds[1].min.x = childBounds[0].max.x;
        childBounds[1].min.y = bounds.min.y;
    } else {
        childBounds[0].max.x = bounds.max.x;
        childBounds[0].max.y = bounds.min.y + bounds.size().y * splitPos;
        childBounds[1].min.x = bounds.min.x;
        childBounds[1].min.y = childBounds[0].max.y;
    }

    if (child[0]) child[0]->updateBounds(childBounds[0]);

    if (child[1]) child[1]->updateBounds(childBounds[1]);
}

bool wagl::ui::SplitLayout::mouseButtonPress(wagl::MouseButton button, glm::vec2 mousePos) {
    for(auto& c : child) {
        if (c->bounds.contains(mousePos)) {
            mouseButtonPress(button, mousePos);
        }
    }

    return true; //Always capture layout to avoid click through.
}

void wagl::ui::SplitLayout::render(wagl::ui::UIContext &ctx) {

}
