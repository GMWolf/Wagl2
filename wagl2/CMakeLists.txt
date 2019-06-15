add_library(Wagl2
        gl/Buffer.cpp
        gl/Buffer.h
        gl/Object.cpp
        gl/Object.h
        gl/VertexArray.cpp
        gl/VertexArray.h
        AttributeTypes.cpp
        AttributeTypes.h
        gl/Shader.cpp
        gl/Shader.h
        util/FlatMap.h
        gl/Draw.cpp
        gl/Draw.h
        Window.cpp
        Window.h
        util/files.h
        util/files.cpp
        Camera.cpp
        Camera.h
        Transform.h
        Transform.cpp
        shaders/fullscreen.h
        gl/Texture.cpp
        gl/Texture.h
        TextureLoad.cpp
        TextureLoad.h
        UniformBuffer.h
        ModelLoad.cpp
        ModelLoad.h
        Model.cpp
        Model.h
        ttinygltf_impl.cpp
        util/Utils.h
        gl/FrameBuffer.cpp
        gl/FrameBuffer.h
        PingPongBuffer.cpp
        PingPongBuffer.h
        gl/Fence.cpp
        gl/Fence.h
        util/overloaded.h
        ui/Layout.h Geom.h InputProcessor.h Application.cpp Application.h ui/Layout.cpp ui/UIRenderBuffer.cpp ui/UIRenderBuffer.h SparseTextureAtlas.cpp SparseTextureAtlas.h util/Matrix.h VirtualTexture.cpp VirtualTexture.h ui/UI.h)

target_link_libraries(Wagl2 PUBLIC glfw glad glm gli tinygltf stb)
target_include_directories(Wagl2 PUBLIC .)