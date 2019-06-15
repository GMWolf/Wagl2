//
// Created by felix on 16/01/2019.
//

#include <glad/glad.h>
#include "Window.h"
#include <string>
#include <iostream>
#include <exception>

wagl::Context::~Context() {
    if (window) {
        glfwDestroyWindow(window);
    }
}

wagl::Context::Context(wagl::Context &&other) noexcept {
    if (window) {
        glfwDestroyWindow(window);
    }
    width = other.width;
    height = other.height;
    window = other.window;
    other.window = nullptr;
}

void wagl::Context::makeContextCurrent() {
    glfwMakeContextCurrent(window);
}

wagl::Context::Context(wagl::ContextHint *hints, size_t hintCount, int width, int height, const char *title, const wagl::Context *shared) :
        width(width), height(height) {

    glfwDefaultWindowHints();
    for(int i = 0; i < hintCount; i++) {
        glfwWindowHint(hints[i].hint, hints[i].value);
    }
    //set version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    window = glfwCreateWindow(width, height, title, NULL, shared? shared->window : nullptr);

    if(!window) return;

    glfwSetWindowUserPointer(window, this);

    glfwSetWindowSizeCallback(window, _resizeCallback);

    if (!shared) {
        makeContextCurrent();
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    }
}

void wagl::Context::_resizeCallback(GLFWwindow *window, int width, int height) {
    auto* ctx = static_cast<Context*>(glfwGetWindowUserPointer(window));
    ctx->width = width;
    ctx->height = height;
    if (ctx->resizeCallback) ctx->resizeCallback(ctx, width, height);
}

void wagl::Context::_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto* ctx = static_cast<Context*>(glfwGetWindowUserPointer(window));
    if(ctx->keyCallback) ctx->keyCallback(ctx, key, scancode, action, mods);
}

void wagl::Context::_charCallback(GLFWwindow *window, unsigned int codepoint) {
    auto* ctx = static_cast<Context*>(glfwGetWindowUserPointer(window));
    if(ctx->charCallback) ctx->charCallback(ctx, codepoint);
}

void wagl::Context::_mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    auto* ctx = static_cast<Context*>(glfwGetWindowUserPointer(window));
    if(ctx->mouseButtonCallback) ctx->mouseButtonCallback(ctx, button, action, mods);
}

void wagl::Context::_dropCallback(GLFWwindow *window, int count, const char **paths) {
    auto* ctx = static_cast<Context*>(glfwGetWindowUserPointer(window));
    if(ctx->dropCallback) ctx->dropCallback(ctx, count, paths);
}

int wagl::Context::getKey(int key) {
    return glfwGetKey(window, key);
}

int wagl::Context::getMouseButton(int button) {
    return glfwGetMouseButton(window, button);
}

glm::vec2 wagl::Context::getMousePos() {
    double posx, posy;
    glfwGetCursorPos(window, &posx, &posy);
    return glm::vec2(posx, posy);
}

void wagl::Context::setMousePos(glm::vec2 pos) {
    glfwSetCursorPos(window, pos.x, pos.y);
}

bool wagl::Context::shouldClose() {
    return static_cast<bool>(glfwWindowShouldClose(window));
}

void wagl::Context::close() {
    glfwSetWindowShouldClose(window, true);
}

bool wagl::Context::opened() {
    return window != nullptr;
}



wagl::Window::Window(int width, int height, const char* title, const Context* shared) :
    Context(nullptr, 0, width, height, title, shared){
}


void wagl::Window::swapBuffers() {
    glfwSwapBuffers(window);
}

struct opengl_error : public std::runtime_error {
    explicit opengl_error(const char* what) : std::runtime_error(what){};
};

#ifdef GLAD_DEBUG
void post_call_callback(const char *name, void *funcptr, int len_args, ...) {
    GLenum err;
    while((err = glad_glGetError()) != GL_NO_ERROR) {

        std::string errorName;

#define ERROR_NAME_CASE(x) case x : errorName = #x; break;

        switch (err) {
            ERROR_NAME_CASE(GL_INVALID_ENUM)
            ERROR_NAME_CASE(GL_INVALID_VALUE)
            ERROR_NAME_CASE(GL_INVALID_OPERATION)
            ERROR_NAME_CASE(GL_STACK_OVERFLOW)
            ERROR_NAME_CASE(GL_STACK_UNDERFLOW)
            ERROR_NAME_CASE(GL_OUT_OF_MEMORY)
            ERROR_NAME_CASE(GL_INVALID_FRAMEBUFFER_OPERATION)
            ERROR_NAME_CASE(GL_CONTEXT_LOST)

            default: errorName = "<Unknown error>";
        }

#undef ERROR_NAME_CASE
        (errorName += " in ") += name;
        throw opengl_error(errorName.c_str());
    }

}

#endif //GLAD_DEBUG

bool wagl::init() {
#ifdef GLAD_DEBUG
    glad_set_post_callback(post_call_callback);
#endif //GLAD_DEBUG
    return glfwInit();
}

void wagl::termintate() {
    return glfwTerminate();
}

void wagl::pollEvents() {
    glfwPollEvents();
}

wagl::OffscreenContext::OffscreenContext(const Context* shared) :
    Context((ContextHint*)hints, 1, 1, 1, "", shared){
}
