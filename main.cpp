#include "Camera.hpp"
#include "Input.hpp"
#include "Profiler.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"
#include "Simulation.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <fmt/core.h>

static void errorCallback(int error, const char* description) {
    fmt::print("Error {}: {}", error, description);
}

const char* algorithms[] = {"Sines", "Gerstner", "DFT", "FFT", "Slow Gerstner", "Slow FFT"};

int main() {
    if (!glfwInit()) {
        fmt::print("GLFW initialization failed.\n");
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Capstone", nullptr, nullptr);
    if (!window) {
        fmt::print("Window creation failed.\n");
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
        fmt::print("Error: failed to initialize gl3w.\n");
        glfwTerminate();
        return -1;
    }

    int size     = 256;
    int prevSize = 256;

    loadShaders();
    loadImages(size);
    loadTextures(size);

    Profiler::initialize();

    Camera camera;
    camera.setPerspective(45.f, 1280.f / 720.f);
    camera.setView({0.f, 0.f, -1.f}, {0.f, 0.f, 0.f});
    camera.setSpeed(3.f);
    camera.setSensitivity(5.f);

    Simulation simulation(size);
    simulation.setAlgorithm(Algorithm::Gerstner);
    simulation.initialize();

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    auto& program = ResourceManager::getProgram("ocean");
    program.setUniform("spacing", simulation.getSpacing());
    program.setUniform("view", camera.getView());
    program.setUniform("model", simulation.getTransform());
    program.setUniform("projection", camera.getProjection());
    program.setUniform("wireframe", false);
    program.setUniform("azimuth", glm::radians(25.0));
    program.setUniform("inclination", glm::radians(35.0));

    auto& debugNormalsProgram = ResourceManager::getProgram("debugNormals");
    debugNormalsProgram.setUniform("spacing", simulation.getSpacing());
    debugNormalsProgram.setUniform("view", camera.getView());
    debugNormalsProgram.setUniform("model", simulation.getTransform());
    debugNormalsProgram.setUniform("projection", camera.getProjection());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    int algo                = 2;
    bool wireframe          = false;
    bool shouldResize       = false;
    bool shouldReinitialize = false;

    float profileTime    = 1.f;
    float windSpeed      = 25.0f;
    float windDirection  = 0.0f;
    auto& spectrumParams = simulation.params();

    float azimuthAngle = glm::radians(25.0);
    float inclinationAngle = glm::radians(35.0);

    bool pause{};
    double time{};
    double prev = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double now       = glfwGetTime();
        double deltaTime = now - prev;
        prev             = now;

        if (!pause) {
            time += deltaTime;
        }

        Profiler::frameBegin();

        if (shouldReinitialize) {
            if (shouldResize) {
                simulation.setSize(size);
                shouldResize = false;
            }

            simulation.initialize();
            shouldReinitialize = false;
            time = 0.0;
        }

        // TODO: use double everywhere for time
        simulation.update(static_cast<float>(time));

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        camera.setPerspective(45.f, static_cast<float>(width) / glm::max(1, height));
        camera.update(deltaTime);

        program.setUniform("view", camera.getView());
        debugNormalsProgram.setUniform("view", camera.getView());
        program.setUniform("cameraPosition", glm::vec4(camera.getPosition(), 1.f));

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Settings");
        ImGui::SeparatorText("Ocean settings");
        ImGui::SeparatorText("Simulation Parameters");
        auto textInputFlags = ImGuiInputTextFlags_EnterReturnsTrue;

        ImGui::PushItemWidth(100.f);
        if (ImGui::Combo("Algorithm", &algo, algorithms, 6)) {
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
            case 5:
                simulation.setAlgorithm(Algorithm::SlowGerstner);
                break;
            }
        }

        if (ImGui::InputInt("Simulation size", &size, 0, 0, textInputFlags)) {
            float sizeLog2;
            sizeLog2 = glm::log2(static_cast<float>(size));
            sizeLog2 = glm::round(sizeLog2);

            size = glm::pow(2, sizeLog2);
            if (size != prevSize) {
                shouldResize = true;
                prevSize     = size;
            }
        }
        ImGui::PopItemWidth();

        ImGui::Dummy({100.f, 5.f});
        ImGui::SeparatorText("Spectrum Parameters");

        ImGui::PushItemWidth(100.f);
        ImGui::InputFloat("Height multiplier", &spectrumParams.A, 0.0f, 0.0f, "%.1f");
        ImGui::InputFloat("Patch size", &spectrumParams.patchSize, 0.0f, 0.0f, "%.1f");
        ImGui::InputFloat("Low Cutoff", &spectrumParams.lowCutoff, 0.0f, 0.0f, "%.3f");
        if (ImGui::InputFloat("Wind speed", &windSpeed, 0.0f, 0.0f, "%.1f")) {
            auto windDir        = glm::radians(windDirection);
            auto wind           = glm::vec2(glm::cos(windDir), glm::sin(windDir)) * windSpeed;
            spectrumParams.wind = wind;
        }
        if (ImGui::InputFloat("Wind direction", &windDirection, 0.0f, 0.0f, "%.1f")) {
            auto windDir        = glm::radians(windDirection);
            auto wind           = glm::vec2(glm::cos(windDir), glm::sin(windDir)) * windSpeed;
            spectrumParams.wind = wind;
        }
        ImGui::PopItemWidth();

        if (ImGui::Button("Reinitialize")) {
            shouldReinitialize = true;
        }

        ImGui::Dummy({100.f, 10.f});
        ImGui::SeparatorText("Light Settings");
        ImGui::PushItemWidth(100.f);
        if (ImGui::SliderAngle("Azimuth", &azimuthAngle, 0.f, 360.f, "%.1f")) {
            program.setUniform("azimuth", azimuthAngle);
        }
        if (ImGui::SliderAngle("Inclination", &inclinationAngle, 0.f, 90.f, "%.1f")) {
            program.setUniform("inclination", inclinationAngle);
        }
        ImGui::PopItemWidth();

        ImGui::Dummy({100.f, 40.f});
        ImGui::SeparatorText("Profiling");
        ImGui::PushItemWidth(100.f);
        ImGui::InputFloat("Profile time", &profileTime, 0.0f, 0.0f, "%.1f");
        ImGui::PopItemWidth();
        if (ImGui::Button("Profile")) {
            if (!Profiler::profiling() && Profiler::resultsAvailable()) {
                shouldReinitialize = true;
                Profiler::beginProfiling(algorithms[algo], profileTime);
            }
        }

        ImGui::End();

        ImGui::Begin("Debug");

        if (ImGui::Button("Play/Pause")) {
            pause = !pause;
        }
        if (ImGui::Button("Toggle wireframe")) {
            wireframe        = !wireframe;
            auto polygonMode = (wireframe) ? GL_LINE : GL_FILL;
            glPolygonMode(GL_FRONT, polygonMode);

            program.setUniform("wireframe", wireframe);
        }
        ImGui::SameLine();
        if (ImGui::Button("Debug Normals")) {
            simulation.toggleDebug();
        }

        ImGui::Image(ResourceManager::getTexture("normal"), {256, 256}, {0, 1}, {1, 0});
        ImGui::Image(ResourceManager::getTexture("displacement"), {256, 256}, {0, 1}, {1, 0});

        ImGui::End();

        ImGui::Render();

        Profiler::queryBegin("DrawOceanSurface");
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        simulation.draw();
        Profiler::queryEnd();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();

        Profiler::frameEnd();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}