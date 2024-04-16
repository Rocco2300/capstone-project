#include "DFT.hpp"

#include "Shader.hpp"
#include "Globals.hpp"

#include <GL/gl3w.h>

DFT::DFT(int size) {
    m_size = size;

    ComputeShader idftShader;
    idftShader.load("../shaders/IDFT.comp");
    m_IDFT.attachShader(idftShader);
    m_IDFT.validate();
    m_IDFT.setUniform("size", size);

    ComputeShader sinesShader;
    sinesShader.load("../shaders/Sines.comp");
    m_sines.attachShader(sinesShader);
    m_sines.validate();
    m_sines.setUniform("size", size);

    ComputeShader gerstnerShader;
    gerstnerShader.load("../shaders/Gerstner.comp");
    m_gerstner.attachShader(gerstnerShader);
    m_gerstner.validate();
    m_gerstner.setUniform("size", size);
}

void DFT::dispatchSines() {
    m_sines.use();

    m_sines.setUniform("direction", 0);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_sines.setUniform("direction", 1);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void DFT::dispatchGerstner() {
    m_gerstner.use();

    m_gerstner.setUniform("direction", 0);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_gerstner.setUniform("direction", 1);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void DFT::dispatchIDFT(int input, int output) {
    m_IDFT.use();
    m_IDFT.setUniform("buffer0", input);
    m_IDFT.setUniform("buffer1", output);

    m_IDFT.setUniform("direction", 0);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_IDFT.setUniform("direction", 1);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void DFT::setSize(int size) {
    m_size = size;

    m_IDFT.setUniform("size", size);
    m_sines.setUniform("size", size);
    m_gerstner.setUniform("size", size);
}