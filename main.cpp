#include "Camera.hpp"
#include "Globals.hpp"
#include "Input.hpp"
#include "Noise.hpp"
#include "Plane.hpp"
#include "Program.hpp"
#include "Shader.hpp"
#include "TextureManager.hpp"
#include "DFT.hpp"
#include "Spectrum.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

static void errorCallback(int error, const char *description) {
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
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Capstone", nullptr, nullptr);
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

    int size = 128;

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

    //TextureManager textureManager;
    Noise noise(size, size);
    //textureManager.insert("noise", size, NOISE_BINDING, true).setData(noise.data());
    //textureManager.insert("normal", size, NORMAL_UNIT);
    //textureManager.insert("displacement", size, DISPLACEMENT_UNIT);
    //textureManager.insert("H0K", size, H0K_BINDING, true);
    //textureManager.insert("H0", size, H0_BINDING);
    //textureManager.insert("buffer", size, BUFFER_BINDING);
    //textureManager.insert("wavedata", size, WAVEDATA_BINDING);
    //textureManager.insert("dy", size, DY_BINDING, true);
    //textureManager.insert("dyx_dyz", size, DYX_DYZ_BINDING, true);

    uint32 textureViewID;
    uint32 textureArrayID;
    glGenTextures(1, &textureArrayID);
    glActiveTexture(GL_TEXTURE0 + BUFFERS_UNIT);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, size, size, 12, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(BUFFERS_UNIT, textureArrayID, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

    glTextureSubImage3D(textureArrayID, 0, 0, 0, NOISE_INDEX, size, size, 1, GL_RGBA, GL_FLOAT, noise.data());

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    glGenTextures(1, &textureViewID);
    glActiveTexture(GL_TEXTURE0 + DEBUG_VIEW_UNIT);
    glBindTexture(GL_TEXTURE_2D, textureViewID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(DEBUG_VIEW_UNIT, textureViewID, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

    glBindTexture(GL_TEXTURE_2D, 0);

    SpectrumParameters params{};
    params.scale = 1.0f;
    params.angle = 172.0f / 180.f * glm::pi<float>();
    params.depth = 1000.0f;
    params.fetch = 8000.0f;
    params.gamma = 3.3f;
    params.patchSize = 250.f;
    params.windSpeed = 75.f;
    params.spreadBlend = 0.25f;

    DFT dft(size);
    Spectrum spectrum(size, params);
    spectrum.initialize();

    VertexShader vertexShader("../shaders/ocean_surface.vert");
    FragmentShader fragmentShader("../shaders/ocean_surface.frag");
    Program program;
    program.attachShader(vertexShader);
    program.attachShader(fragmentShader);
    program.validate();
    program.use();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    //program.setUniform("size", size);
    //glBindTexture(GL_TEXTURE_2D, textureManager.get("displacement"));
    //program.setUniform("displacement", DISPLACEMENT_BINDING);
    //glBindTexture(GL_TEXTURE_2D, textureManager.get("normal"));
    //program.setUniform("normal", NORMAL_BINDING);
    //program.setUniform("view", camera.getView());
    //program.setUniform("model", oceanPlane.getTransform());
    //program.setUniform("projection", camera.getProjection());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    float prev = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float now = glfwGetTime();
        float deltaTime = now - prev;
        prev = now;

        spectrum.update(now);
        dft.dispatchIDFT();

        //program.use();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        camera.setPerspective(45.f, static_cast<float>(width) / glm::max(1, height));
        camera.update(deltaTime);

        //program.setUniform("view", camera.getView());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug");

        //ImGui::Image(textureManager.get("dyx_dyz"), {256, 256}, {0, 1}, {1, 0});
        //ImGui::Image(textureManager.get("displacement"), {256, 256}, {0, 1}, {1, 0});
        //ImGui::Image(textureManager.get("normal"), {256, 256}, {0, 1}, {1, 0});
        //ImGui::Image(textureManager.get("buffer"), {256, 256}, {0, 1}, {1, 0});
        //ImGui::Image(textureManager.get("dy"), {256, 256}, {0, 1}, {1, 0});

        ImGui::End();
        ImGui::Render();

        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //oceanPlane.bind();
        //glDrawElements(GL_TRIANGLES, oceanPlane.getIndices().size(), GL_UNSIGNED_INT, 0);
        //oceanPlane.unbind();

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