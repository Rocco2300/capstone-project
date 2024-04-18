#include "Camera.hpp"
#include "Globals.hpp"
#include "Image.hpp"
#include "Input.hpp"
#include "Plane.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"
#include "Simulation.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Capstone", nullptr, nullptr);
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

    int size = 256;

    Plane oceanPlane;
    oceanPlane.setSpacing(0.25f);
    oceanPlane.generate(size, size);
    oceanPlane.setOrigin({oceanPlane.getSize().x / 2, 0.f, oceanPlane.getSize().y / 2});
    oceanPlane.setPosition({0.f, -2.f, 0.f});

    Camera camera;
    camera.setPerspective(45.f, 1280.f / 720.f);
    camera.setView({0.f, 0.f, -1.f}, {0.f, 0.f, 0.f});
    camera.setSpeed(3.f);
    camera.setSensitivity(100.f);

    ResourceManager::insertTexture("buffers", BUFFERS_UNIT, size, 14);
    ResourceManager::insertTexture("displacement", DISPLACEMENT_UNIT, size);
    ResourceManager::insertTexture("normal", NORMAL_UNIT, size);
    ResourceManager::insertTexture("test", DEBUG_VIEW_UNIT, size);

    Simulation simulation(size);
    simulation.setAlgorithm(Algorithm::Gerstner);

    VertexShader vertexShader("../shaders/Ocean.vert");
    FragmentShader fragmentShader("../shaders/Ocean.frag");
    Program program;
    program.attachShader(vertexShader);
    program.attachShader(fragmentShader);
    program.validate();
    program.use();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    program.setUniform("view", camera.getView());
    program.setUniform("model", oceanPlane.getTransform());
    program.setUniform("projection", camera.getProjection());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    int algo = 1;
    float prev = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float now       = glfwGetTime();
        float deltaTime = now - prev;
        prev            = now;

        simulation.update(now);

        program.use();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        camera.setPerspective(45.f, static_cast<float>(width) / glm::max(1, height));
        camera.update(deltaTime);

        program.setUniform("view", camera.getView());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug");

        if (ImGui::InputInt("Algorithm", &algo)) {
            algo %= 5;
            switch (algo) {
            case 0:
                simulation.setAlgorithm(Algorithm::Sines);
                break;
            case 1:
                simulation.setAlgorithm(Algorithm::Gerstner);
                break;
            case 2:
                simulation.setAlgorithm(Algorithm::DFT);
                break;
            case 3:
                simulation.setAlgorithm(Algorithm::FFT);
                break;
            case 4:
                simulation.setAlgorithm(Algorithm::SlowGerstner);
                break;
            }
        }

        ImGui::Image(ResourceManager::getTexture("test"), {256, 256}, {0, 1}, {1, 0});
        ImGui::Image(ResourceManager::getTexture("normal"), {256, 256}, {0, 1}, {1, 0});
        ImGui::Image(ResourceManager::getTexture("displacement"), {256, 256}, {0, 1}, {1, 0});

        ImGui::End();
        ImGui::Render();

        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        oceanPlane.bind();
        glDrawElements(GL_TRIANGLES, oceanPlane.getIndices().size(), GL_UNSIGNED_INT, 0);
        oceanPlane.unbind();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}