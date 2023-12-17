#include "Camera.hpp"
#include "Plane.hpp"
#include "Program.hpp"
#include "Shader.hpp"
#include "Input.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <iostream>

static void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << '\n';
}

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed.\n";
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    GLFWwindow* window = glfwCreateWindow(800, 800, "Capstone", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(errorCallback);
    glfwSetKeyCallback(window, Keyboard::callback);
    glfwSetCursorPosCallback(window, Mouse::posCallback);
    glfwSetMouseButtonCallback(window, Mouse::buttonCallback);
    Mouse::setWindow(window);

    if (gl3wInit()) {
        std::cerr << "Error: failed to initialize gl3w.\n";
        glfwTerminate();
        return -1;
    }

    Plane mesh;
    mesh.generate(2, 2);
    mesh.setSpacing(0.5f);
    //    mesh.setPosition({-0.25f, 0.f, 0.f});
    mesh.setOrigin({mesh.getSize().x / 2, 0.f, mesh.getSize().y / 2});
    mesh.setRotation({-90.f, 0.f, 0.f});

    Camera camera;
    camera.setPerspective(45.f, 1.f);
    camera.setView({0.f, 0.f, -1.f}, {0.f, 0.f, 0.f});
    camera.setSpeed(3.f);
    camera.setSensitivity(100.f);

    VertexShader vertexShader("../shaders/ocean_surface.vert");
    FragmentShader fragmentShader("../shaders/ocean_surface.frag");
    Program program;
    program.attachShader(vertexShader);
    program.attachShader(fragmentShader);
    program.validate();
    program.use();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    auto mvp = camera.getPerspective() * camera.getView() * mesh.getTransform();
    program.setUniform("mvp", mvp);

    float prev = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float now = glfwGetTime();
        float deltaTime = now - prev;
        prev = now;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        camera.update(deltaTime);

        auto mvp = camera.getPerspective() * camera.getView() * mesh.getTransform();
        program.setUniform("mvp", mvp);

        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mesh.bind();
        glDrawElements(GL_TRIANGLES, mesh.getIndices().size(), GL_UNSIGNED_INT, 0);
        mesh.unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}