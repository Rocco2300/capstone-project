cmake_minimum_required(VERSION 3.23)

project(capstone-project)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
file(GLOB HEADERS include/*.hpp)
add_executable(${PROJECT_NAME} main.cpp ${HEADERS})

add_subdirectory(src)
target_link_libraries(${PROJECT_NAME} src glfw3 glew32)