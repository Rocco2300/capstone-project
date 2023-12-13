cmake_minimum_required(VERSION 3.23)

project(capstone-project)

set(CMAKE_CXX_STANDARD 20)

find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

include_directories(include)
file(GLOB HEADERS include/*.hpp)
add_executable(${PROJECT_NAME} main.cpp ${HEADERS})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)
target_link_libraries(${PROJECT_NAME} src glfw glew32)