#include "Simulation.hpp"

#include <GL/gl3w.h>

void Simulation::resize(int width, int height) {
    m_surface->generate(width, height);

    m_normal->setSize(width, height);
    m_displacement->setSize(width, height);
}

void Simulation::setSurface(Plane& surface) {
    m_surface = &surface;
}

void Simulation::setNormal(Texture& normal) {
    m_normal = &normal;
}

void Simulation::setDisplacement(Texture& displacement) {
    m_displacement = &displacement;
}

void SineSimulation::init() {
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

void SineSimulation::update() {
    m_updateProgram.use();
    glDispatchCompute(512, 512, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}