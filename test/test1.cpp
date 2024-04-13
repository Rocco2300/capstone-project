#include "Shader.hpp"
#include "Program.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

const int width = 4;
const int height = 16;

unsigned int reversed[height];

void createTexture(unsigned int id, int binding) {
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0 + binding);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(binding, id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int* computeReversals() {
    auto* res = new unsigned int[height];
    for (int i = 0; i < height; i++) {
        int index = i;
        unsigned int num = 0;
        for (int j = 0; j < width; j++) {
            num = (num << 1) + (index & 1);
            index >>= 1;
        }
        res[i] = num;
    }
    return res;
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
    if (gl3wInit()) {
        std::cerr << "Error: failed to initialize gl3w.\n";
        glfwTerminate();
        return -1;
    }

    unsigned int indexes;
    unsigned int twiddle;
    createTexture(twiddle, 0);
    createTexture(indexes, 1);

    Program program1;
    ComputeShader shader1("../test/twiddle.comp");
    program1.attachShader(shader1);
    program1.validate();
    program1.use();
    glDispatchCompute(width, height / 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    auto* reversal = computeReversals();
    unsigned int ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * height, reversal, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    delete[] reversal;

    Program program2;
    ComputeShader shader2("../test/indexes.comp");
    program2.attachShader(shader2);
    program2.validate();
    program2.use();
    glDispatchCompute(width, height / 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug");

        ImGui::Image(reinterpret_cast<ImTextureID>(twiddle), {256, 256}, {0, 1}, {1, 0});
        ImGui::Image(reinterpret_cast<ImTextureID>(indexes), {256, 256}, {0, 1}, {1, 0});

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