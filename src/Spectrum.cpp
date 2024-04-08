#include "Spectrum.hpp"

#include "Shader.hpp"
#include "Globals.hpp"

#include "GL/gl3w.h"

Spectrum::Spectrum(int size, const SpectrumParameters& params) {
    m_size = size;

    ComputeShader spectrumShader;
    spectrumShader.load("../shaders/InitialSpectrum.comp");
    m_initialProgram.attachShader(spectrumShader);
    m_initialProgram.validate();
    m_initialProgram.use();
    m_initialProgram.setUniform("size", m_size);

    glGenBuffers(1, &m_paramsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_paramsSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(params), &params, GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, PARAMS_BINDING, m_paramsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    ComputeShader timeDependentShader;
    timeDependentShader.load("../shaders/TimeDependentSpectrum.comp");
    m_timeDependentProgram.attachShader(timeDependentShader);
    m_timeDependentProgram.validate();
}

void Spectrum::initialize() {
    m_initialProgram.setUniform("conjugate", 0);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_initialProgram.setUniform("conjugate", 1);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Spectrum::update(double time) {
    m_timeDependentProgram.setUniform("time", time);
    m_timeDependentProgram.use();
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}