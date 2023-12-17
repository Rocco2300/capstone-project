cmake_minimum_required(VERSION 3.23)

project(capstone-project)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
include_directories(external/glm)
file(GLOB HEADERS include/*.hpp)
add_executable(${PROJECT_NAME} main.cpp ${HEADERS})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)
add_subdirectory(external/fmt)
add_subdirectory(external/glm)
add_subdirectory(external/glfw)
add_subdirectory(external/gl3w)
target_link_libraries(${PROJECT_NAME} src glfw gl3w)