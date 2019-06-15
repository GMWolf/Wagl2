//
// Created by felix on 13/06/2019.
//

#include <Application.h>
#include <ui/UIRenderBuffer.h>
#include <ui/Layout.h>

void run() {
    wagl::AppConfig config {
            1280, 720,
            "UI demo"
    };

    wagl::Application app(config);

    wagl::ui::UIContext uictx;

    wagl::ui::SplitLayout splitLayout;

    app.inputMultiplexer.add(&splitLayout);

    app.window.resizeCallback = [&](wagl::Context*, int width, int height){
        splitLayout.updateBounds({{0,0}, {width, height}});
    };

    app.run([&](float dt){
        splitLayout.render(uictx);
    });
}

int main() {
    wagl::init();

    run();

    wagl::termintate();

    return 0;
}