#include "Camera.hpp"
#include "Plane.hpp"
#include "Shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>

static void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << '\n';
}

static void keyCallback(GLFWwindow* window,
                        int key,
                        int scancode,
                        int action,
                        int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed.\n";
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    GLFWwindow* window =
            glfwCreateWindow(480, 480, "Capstone", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(errorCallback);
    glfwSetKeyCallback(window, keyCallback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed.\n";
        glfwTerminate();
        return -1;
    }

    Plane mesh;
    mesh.generate(1, 1);
//    mesh.setPosition({-0.25f, 0.f, 0.f});
    mesh.setOrigin({.25f, .25f, 0.f});
 //   mesh.setRotation({0.f, 0.f, 45.f});

    Camera camera;
    camera.setView({0.f, 0.f, 1.f}, {0.f, 0.f, -1.f});
    camera.setPerspective(45.f, 1.f, 0.1f, 100.f);

    Shader shader("../shaders/ocean_surface.vert",
                  "../shaders/ocean_surface.frag");
    shader.use();
    auto mvpLocation = glGetUniformLocation(shader.getId(), "mvp");
    auto mvp = camera.getPerspective() * camera.getView() * mesh.getTransform();
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        mesh.bind();
        glDrawElements(GL_TRIANGLES, mesh.getIndices().size(), GL_UNSIGNED_INT,
                       0);
        mesh.unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}