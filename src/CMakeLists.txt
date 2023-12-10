add_library(src
        Mesh.cpp
        Camera.cpp
        Transformable.cpp
        Shader.cpp
        Plane.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

target_link_libraries(src glew32 opengl32)