add_library(src
        Mesh.cpp
        Camera.cpp
        Transformable.cpp
        Program.cpp
        Plane.cpp
        Shader.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

target_link_libraries(src glew32 opengl32)