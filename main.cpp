#include "Camera.hpp"
#include "Input.hpp"
#include "Plane.hpp"
#include "Program.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Noise.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

struct Params
{
    float scale;
    float angle;
    float depth;
    float fetch;
    float gamma;
    float swell;
    float windSpeed;
    float spreadBlend;
};

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

    Plane oceanPlane;
    oceanPlane.setSpacing(0.25f);
    oceanPlane.generate(512, 512);
    oceanPlane.setOrigin({oceanPlane.getSize().x / 2, 0.f, oceanPlane.getSize().y / 2});
    oceanPlane.setPosition({0.f, -2.f, 0.f});

    Camera camera;
    camera.setPerspective(45.f, 1280.f / 720.f);
    camera.setView({0.f, 0.f, -1.f}, {0.f, 0.f, 0.f});
    camera.setSpeed(3.f);
    camera.setSensitivity(100.f);

    Texture displacement;
    displacement.setSize(512, 512);
    displacement.setFormat(GL_RGBA32F, GL_RGBA, GL_FLOAT);

    Texture normal;
    normal.setSize(512, 512);
    normal.setFormat(GL_RGBA32F, GL_RGBA, GL_FLOAT);

    Noise noiseImage(512, 512);
    Texture noise;
    noise.setSize(512, 512);
    noise.setFormat(GL_RG32F, GL_RG, GL_FLOAT);
    noise.setData(noiseImage.data());

    Texture h0K;
    h0K.setSize(512, 512);
    h0K.setFormat(GL_RG32F, GL_RG, GL_FLOAT);

    Texture h0;
    h0.setSize(512, 512);
    h0.setFormat(GL_RGBA32F, GL_RGBA, GL_FLOAT);

    Program spectrumProgram;
    ComputeShader spectrumShader("../shaders/InitialSpectrum.comp");
    spectrumProgram.attachShader(spectrumShader);
    spectrumProgram.validate();
    spectrumProgram.use();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noise);
    glBindImageTexture(0, noise, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, h0K);
    glBindImageTexture(1, h0K, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, h0);
    glBindImageTexture(2, h0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    Params params{};
    params.angle = 172.0f / 180.f * glm::pi<float>();
    params.depth = 100.0f;
    params.fetch = 1000.0f;
    params.gamma = 3.3f;
    params.swell = 0.01f;
    params.windSpeed = 10.f;
    params.spreadBlend = 0.25f;

    uint32 paramsSSBO;
    glGenBuffers(1, &paramsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, paramsSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(params), &params, GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 16, paramsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    spectrumProgram.setUniform("conjugate", 0);
    glDispatchCompute(512, 512, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    spectrumProgram.setUniform("conjugate", 1);
    glDispatchCompute(512, 512, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    //Program updateProgram;
    //ComputeShader computeShader("../shaders/sine.comp");
    //updateProgram.attachShader(computeShader);
    //updateProgram.validate();

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, displacement);
    //glBindImageTexture(0, displacement, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, normal);
    //glBindImageTexture(1, normal, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

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

    glBindTexture(GL_TEXTURE_2D, displacement);
    program.setUniform("displacement", 0);
    glBindTexture(GL_TEXTURE_2D, normal);
    program.setUniform("normal", 1);
    program.setUniform("view", camera.getView());
    program.setUniform("model", oceanPlane.getTransform());
    program.setUniform("projection", camera.getProjection());

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

        //updateProgram.use();
        //glDispatchCompute(512, 512, 1);
        //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        program.use();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        camera.setPerspective(45.f, static_cast<float>(width) / height);
        camera.update(deltaTime);

        program.setUniform("view", camera.getView());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug");

        ImGui::Image(h0, {256, 256}, {0, 1}, {1, 0});
        ImGui::Image(h0K, {256, 256}, {0, 1}, {1, 0});
        ImGui::Image(noise, {256, 256}, {0, 1}, {1, 0});

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