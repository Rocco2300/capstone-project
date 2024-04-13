cmake_minimum_required(VERSION 3.23)

project(capstone-project)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external/glm)
include_directories(${CMAKE_SOURCE_DIR}/external/imgui)
include_directories(${CMAKE_SOURCE_DIR}/external/glfw/include)
include_directories(${CMAKE_BINARY_DIR}/external/gl3w/include)

file(GLOB HEADERS include/*.hpp)
add_executable(test ${CMAKE_SOURCE_DIR}/test/test.cpp ${HEADERS})
add_executable(test1 ${CMAKE_SOURCE_DIR}/test/test1.cpp ${HEADERS})
add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/main.cpp ${HEADERS})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/fmt)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/glm)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/glfw)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/gl3w)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/imgui)

target_link_libraries(test src glfw gl3w imgui)
target_link_libraries(test1 src glfw gl3w imgui)
target_link_libraries(${PROJECT_NAME} src glfw gl3w imgui)