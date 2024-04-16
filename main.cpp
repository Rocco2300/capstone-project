#include "Camera.hpp"
#include "DFT.hpp"
#include "FFT.hpp"
#include "Globals.hpp"
#include "Input.hpp"
#include "Noise.hpp"
#include "Plane.hpp"
#include "Program.hpp"
#include "Shader.hpp"
#include "Spectrum.hpp"
#include "TextureManager.hpp"

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

    TextureManager textureManager;
    textureManager.insert("displacement", DISPLACEMENT_UNIT, size);
    textureManager.insert("normal", NORMAL_UNIT, size);

    Noise noise(size, size);
    std::vector<float> initialData(4 * size * size, 0);
    auto* texArray = &textureManager.insert("buffers", BUFFERS_UNIT, size, 14);
    texArray->setData(noise.data(), NOISE_INDEX);
    texArray->setData(&initialData[0], HEIGHT_INDEX);
    texArray->setData(&initialData[0], NORMAL_INDEX);
    texArray->setData(&initialData[0], DISPLACEMENT_INDEX);

    auto windSpeed     = 25.0f;
    auto windDirection = glm::pi<float>() / 4.f;
    auto wind          = glm::vec2(glm::cos(windDirection), glm::sin(windDirection)) * windSpeed;
    SpectrumParameters params{};
    params.a         = 4.0f;
    params.patchSize = 1250.0f;
    params.wind      = wind;

    DFT dft(size);
    FFT fft(size);
    Spectrum spectrum(size, params);
    spectrum.initialize();

    Program textureMerger;
    ComputeShader textureMergerShader;
    textureMergerShader.load("../shaders/TextureMerger.comp");
    textureMerger.attachShader(textureMergerShader);
    textureMerger.validate();

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

    float prev = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float now       = glfwGetTime();
        float deltaTime = now - prev;
        prev            = now;

        spectrum.update(now);
        //dft.dispatchIDFT(DY_INDEX, HEIGHT_INDEX);
        //dft.dispatchIDFT(DX_DZ_INDEX, DISPLACEMENT_INDEX);
        //dft.dispatchIDFT(DYX_DYZ_INDEX, NORMAL_INDEX);
        //dft.dispatchSines();
        //dft.dispatchGerstner();
        fft.dispatchIFFT(DY_INDEX, HEIGHT_INDEX);
        fft.dispatchIFFT(DX_DZ_INDEX, DISPLACEMENT_INDEX);
        fft.dispatchIFFT(DYX_DYZ_INDEX, NORMAL_INDEX);
        textureMerger.use();
        glDispatchCompute(size / THREAD_NUMBER, size / THREAD_NUMBER, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

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

        ImGui::Image(textureManager.get("normal"), {256, 256}, {0, 1}, {1, 0});
        ImGui::Image(textureManager.get("displacement"), {256, 256}, {0, 1}, {1, 0});

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