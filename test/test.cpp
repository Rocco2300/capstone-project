#include "Shader.hpp"
#include "Program.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

const int size = 128;
static unsigned int textureViewID;
static unsigned int textureArrayID;

void createTextureArray(int binding) {
    glGenTextures(1, &textureArrayID);
    glActiveTexture(GL_TEXTURE0 + binding);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, size, size, 12, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(binding, textureArrayID, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void createTexture(int binding) {
    glGenTextures(1, &textureViewID);
    glActiveTexture(GL_TEXTURE0 + binding);
    glBindTexture(GL_TEXTURE_2D, textureViewID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(binding, textureViewID, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

    glBindTexture(GL_TEXTURE_2D, 0);
}

struct MinMax {
    float min;
    float max;
};

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
    if (gl3wInit()) {
        std::cerr << "Error: failed to initialize gl3w.\n";
        glfwTerminate();
        return -1;
    }

    createTextureArray(0);
    createTexture(1);

    Program program;
    ComputeShader shader("../test/test.comp");
    program.attachShader(shader);
    program.validate();
    program.use();
    glDispatchCompute(size, size, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    Program textureView;
    ComputeShader textureViewShader("../test/LayerToTexView.comp");
    textureView.attachShader(textureViewShader);
    textureView.validate();
    textureView.setUniform("index", 5);
    textureView.use();
    glDispatchCompute(size, size, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    MinMax m {
            .min = std::numeric_limits<float>::max(),
            .max = std::numeric_limits<float>::min()
    };
    unsigned int ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(m), &m, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    Program minMax;
    ComputeShader minMaxShader("../test/FindMinAndMax.comp");
    minMax.attachShader(minMaxShader);
    minMax.validate();
    minMax.use();
    glDispatchCompute(size, size, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    Program normalize;
    ComputeShader normalizeShader("../test/NormalizeTexView.comp");
    normalize.attachShader(normalizeShader);
    normalize.validate();
    normalize.use();
    glDispatchCompute(size, size, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    int index = 0;
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug");

        if (ImGui::InputInt("Index", &index)) {
            textureView.setUniform("index", index);
            textureView.use();
            glDispatchCompute(size, size, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

            minMax.use();
            glDispatchCompute(size, size, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

            normalize.use();
            glDispatchCompute(size, size, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }

        ImGui::Image(reinterpret_cast<ImTextureID>(textureViewID), {256, 256}, {0, 1}, {1, 0});

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}