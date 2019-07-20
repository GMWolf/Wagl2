//
// Created by felix on 16/01/2019.
//

#ifndef WAGL2_WINDOW_H
#define WAGL2_WINDOW_H

#include <GLFW/glfw3.h>
#include <functional>
#include <glm/vec2.hpp>

namespace wagl {

    //Initializes Wagl (glfw  nad future libraries)
    bool init();

    void termintate();

    void pollEvents();

    struct ContextHint {
        int hint;
        int value;
    };

    class Context {
    public:
        void makeContextCurrent();

        std::function<void(Context*,int,int)> resizeCallback;
        std::function<void(Context*,int,int,int,int)> keyCallback;
        std::function<void(Context*,unsigned int codepoint)> charCallback;
        std::function<void(Context*,int,int,int)> mouseButtonCallback;
        std::function<void(Context*,int,const char**)> dropCallback;

        Context(const Context&) = delete;
        Context(Context&& other) noexcept;

        Context& operator=(const Context&) = delete;
        Context& operator=(Context&&) noexcept = default;

        int getKey(int key) const;
        int getMouseButton(int button) const;
        glm::vec2 getMousePos() const;
        void setMousePos(glm::vec2 pos) const;


        bool shouldClose() const;
        void close();

        bool opened();
    protected:
        int width, height;
        GLFWwindow* window = nullptr;
        Context(ContextHint* hints, size_t hintCount, int width, int height, const char* title, const Context* shared = nullptr);
        ~Context();

        static void _resizeCallback(GLFWwindow* window, int width, int height);
        static void _keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void _charCallback(GLFWwindow* window, unsigned int codepoint);
        static void _mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void _dropCallback(GLFWwindow* window, int count, const char** paths);
    };


    class Window : public Context{

    public:
        Window(int width, int height, const char* title, const Context* shared = nullptr);

        Window(const Window&) = delete;
        Window(Window&&) noexcept = default;

        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = default;

        void swapBuffers();

        int getWidth() const {
            return width;
        }

        int getHeight() const {
            return height;
        }

        float getAspect() const {
            return width / (float) height;
        }

    };


    class OffscreenContext : public Context {
        constexpr static ContextHint hints[] {
                {GLFW_VISIBLE, GLFW_FALSE}
        };
    public:
        explicit OffscreenContext(const Context* shared = nullptr);
    };


}


#endif //WAGL2_WINDOW_H
