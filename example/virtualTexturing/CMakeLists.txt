add_executable(virtualTexturing
        main.cpp
        CameraController.cpp
        CameraController.h
        )
target_link_libraries(virtualTexturing PUBLIC Wagl2)
target_include_directories(virtualTexturing PUBLIC ../resources/shaders)