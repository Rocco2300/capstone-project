#include "Simulation.hpp"

#include "Shader.hpp"
#include "Globals.hpp"

#include <GL/gl3w.h>

#include <glm/gtc/constants.hpp>

Simulation::Simulation(int size)
    : m_size{size}
    , m_spectrum{size}
    , m_dft{size}
    , m_fft{size} {

    std::vector<float> initialData(4 * size * size, 0);
    auto* texArray = &ResourceManager::getTexture("buffers");
    texArray->setData(&initialData[0], HEIGHT_INDEX);
    texArray->setData(&initialData[0], NORMAL_INDEX);
    texArray->setData(&initialData[0], DISPLACEMENT_INDEX);

    auto windSpeed     = 8.0f;
    auto windDirection = glm::pi<float>() / 4.f;
    auto wind          = glm::vec2(glm::cos(windDirection), glm::sin(windDirection)) * windSpeed;
    SpectrumParameters params{};
    params.A         = 4.0f;
    params.patchSize = 250.0f;
    params.wind      = wind;

    m_spectrum.setParameters(params);
    m_spectrum.initialize();

    ComputeShader textureMergerShader;
    textureMergerShader.load("../shaders/TextureMerger.comp");
    m_textureMerger.attachShader(textureMergerShader);
    m_textureMerger.validate();
}

void Simulation::setAlgorithm(Algorithm algorithm) {
    m_algorithm = algorithm;

    if (algorithm == Algorithm::SlowGerstner || algorithm == Algorithm::SlowFFT) {
        m_spectrum.setAccelerated(false);
    } else {
        m_spectrum.setAccelerated(true);
    }
}

void Simulation::update(float time) {
    m_spectrum.update(time);

    switch (m_algorithm) {
    case Algorithm::Sines:
        m_dft.dispatchSines();
        break;
    case Algorithm::Gerstner:
        m_dft.dispatchGerstner();
        break;
    case Algorithm::DFT:
        m_dft.dispatchIDFT(DY_INDEX, HEIGHT_INDEX);
        m_dft.dispatchIDFT(DX_DZ_INDEX, DISPLACEMENT_INDEX);
        m_dft.dispatchIDFT(DYX_DYZ_INDEX, NORMAL_INDEX);
        break;
    case Algorithm::FFT:
        m_fft.dispatchIFFT(DY_INDEX, HEIGHT_INDEX);
        m_fft.dispatchIFFT(DX_DZ_INDEX, DISPLACEMENT_INDEX);
        m_fft.dispatchIFFT(DYX_DYZ_INDEX, NORMAL_INDEX);
        break;
    case Algorithm::SlowGerstner:
        m_dft.dispatchGerstnerCPU();
        break;
    }

    m_textureMerger.use();
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}