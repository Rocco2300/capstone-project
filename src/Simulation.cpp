#include "Simulation.hpp"

#include <GL/gl3w.h>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <random>

void Simulation::resize(int width, int height) {
    m_width = width;
    m_height = height;

    m_normal->setSize(width, height);
    m_surface->generate(width, height);
    m_displacement->setSize(width, height);
}

void Simulation::setSurface(Plane& surface) { m_surface = &surface; }

void Simulation::setNormal(Texture& normal) { m_normal = &normal; }

void Simulation::setDisplacement(Texture& displacement) { m_displacement = &displacement; }

void SineSimulation::init(uint32 width, uint32 height) {
    m_width  = width;
    m_height = height;

    m_surface->generate(m_width, m_height);
    m_surface->setOrigin({m_width / 2, 0.f, m_height / 2});
    m_surface->setPosition({0.f, -2.f, 0.f});
    m_surface->setScale({0.25f, 1.f, 0.25f});

    ComputeShader computeShader("../shaders/sine.comp");
    m_updateProgram.attachShader(computeShader);
    m_updateProgram.validate();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *m_displacement);
    glBindImageTexture(0, *m_displacement, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, *m_normal);
    glBindImageTexture(1, *m_normal, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void SineSimulation::update(double time) {
    m_updateProgram.use();
    m_updateProgram.setFloat("t", time);
    glDispatchCompute(m_width / 32, m_height / 32, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void SineSimulation::generateWaves(uint32 count) {
    auto amplitude = [&](float x) {
        auto normalizedX = (x / count) * 32.f;
        return (glm::pow(0.82f, normalizedX));
    };

    auto frequency = [](float wavelength) {
        return 2.f / wavelength;
    };

    auto phase = [](float speed, float wavelength) {
        return speed * (2.f / wavelength);
    };

    int generalDir = rand() % 361;
    std::vector<Wave> waves;
    for (int i = 0; i < count; i ++) {
        float angle = generalDir + (rand() % 61 - 30);
        glm::vec2 dir{glm::sin(angle), glm::cos(angle)};
        float amp = amplitude(i) * 2.f;
        float wlength = 1024.f * amp;
        float freq = frequency(wlength);
        float p = phase(rand() % 16 + 16, wlength);

        Wave wave{dir, amp, freq, p};
        waves.push_back(wave);
    }

    if (m_wavesSSBO == 0) {
        glGenBuffers(1, &m_wavesSSBO);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_wavesSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, waves.size() * sizeof(Wave), waves.data(),
                 GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_wavesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    m_updateProgram.setInt("waveNumber", waves.size());
}