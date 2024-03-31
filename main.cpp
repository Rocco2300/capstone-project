#include "Camera.hpp"
#include "Globals.hpp"
#include "Input.hpp"
#include "Noise.hpp"
#include "Plane.hpp"
#include "Program.hpp"
#include "Shader.hpp"
#include "TextureManager.hpp"

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

    TextureManager textureManager;
    Noise noise(size, size);
    textureManager.insert("noise", size, NOISE_BINDING, true).setData(noise.data());
    textureManager.insert("normal", size, NORMAL_BINDING);
    textureManager.insert("displacement", size, DISPLACEMENT_BINDING);
    textureManager.insert("H0K", size, H0K_BINDING, true);
    textureManager.insert("H0", size, H0_BINDING);
    textureManager.insert("buffer", size, BUFFER_BINDING, true);
    textureManager.insert("wavedata", size, WAVEDATA_BINDING);
    textureManager.insert("dy", size, DY_BINDING, true);

    Params params{};
    params.scale = 1.0f;
    params.angle = 172.0f / 180.f * glm::pi<float>();
    params.depth = 1000.0f;
    params.fetch = 8000.0f;
    params.gamma = 3.3f;
    params.swell = 0.01f;
    params.windSpeed = 75.f;
    params.spreadBlend = 0.25f;

    Program spectrumProgram;
    ComputeShader spectrumShader;
    spectrumShader.load("../include/Globals.hpp");
    spectrumShader.load("../shaders/InitialSpectrum.comp");
    spectrumProgram.attachShader(spectrumShader);
    spectrumProgram.validate();
    spectrumProgram.use();
    spectrumProgram.setUniform("size", size);

    uint32 paramsSSBO;
    glGenBuffers(1, &paramsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, paramsSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(params), &params, GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, PARAMS_BINDING, paramsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    spectrumProgram.setUniform("conjugate", 0);
    glDispatchCompute(size / THREAD_NUMBER, size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    spectrumProgram.setUniform("conjugate", 1);
    glDispatchCompute(size / THREAD_NUMBER, size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    Program timeDependentProgram;
    ComputeShader timeDependentShader;
    timeDependentShader.load("../include/Globals.hpp");
    timeDependentShader.load("../shaders/TimeDependentSpectrum.comp");
    timeDependentProgram.attachShader(timeDependentShader);
    timeDependentProgram.validate();

    timeDependentProgram.setUniform("time", glfwGetTime());
    timeDependentProgram.use();
    glDispatchCompute(size / THREAD_NUMBER, size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    Program idftProgram;
    ComputeShader idftShader;
    idftShader.load("../include/Globals.hpp");
    idftShader.load("../shaders/IDFT.comp");
    idftProgram.attachShader(idftShader);
    idftProgram.validate();
    idftProgram.setUniform("size", size);

    idftProgram.use();
    idftProgram.setUniform("horizontalPass", 1);
    glDispatchCompute(size / THREAD_NUMBER, size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    idftProgram.setUniform("horizontalPass", 0);
    glDispatchCompute(size / THREAD_NUMBER, size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

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

    program.setUniform("size", size);
    glBindTexture(GL_TEXTURE_2D, textureManager.get("displacement"));
    program.setUniform("displacement", DISPLACEMENT_BINDING);
    glBindTexture(GL_TEXTURE_2D, textureManager.get("normal"));
    program.setUniform("normal", NORMAL_BINDING);
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

        timeDependentProgram.setUniform("time", glfwGetTime());
        timeDependentProgram.use();
        glDispatchCompute(size / THREAD_NUMBER, size / THREAD_NUMBER, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        idftProgram.use();
        idftProgram.setUniform("horizontalPass", 1);
        glDispatchCompute(size / THREAD_NUMBER, size / THREAD_NUMBER, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        idftProgram.setUniform("horizontalPass", 0);
        glDispatchCompute(size / THREAD_NUMBER, size / THREAD_NUMBER, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

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

        ImGui::Image(textureManager.get("displacement"), {256, 256}, {0, 1}, {1, 0});
        ImGui::Image(textureManager.get("buffer"), {256, 256}, {0, 1}, {1, 0});
        ImGui::Image(textureManager.get("dy"), {256, 256}, {0, 1}, {1, 0});

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