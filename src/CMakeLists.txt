add_library(src
        Mesh.cpp
        Camera.cpp
        Transformable.cpp
        Program.cpp
        Plane.cpp
        Shader.cpp
        Input.cpp
        Texture.cpp
        Image.cpp
        TextureManager.cpp
        DFT.cpp
        Spectrum.cpp
        FFT.cpp
        GPUSimulation.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/include)

target_link_libraries(src fmt glfw gl3w)