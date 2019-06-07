set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(glm)

add_subdirectory(gli)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)

add_subdirectory(glfw)

add_library(stb stb_impl.cpp)
target_include_directories(stb PUBLIC stb)

add_library(tinygltf INTERFACE)
target_sources(tinygltf INTERFACE tinygltf/tiny_gltf.h)
target_include_directories(tinygltf INTERFACE tinygltf)