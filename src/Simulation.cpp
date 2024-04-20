#include "Simulation.hpp"

#include "Globals.hpp"
#include "Profiler.hpp"
#include "ResourceManager.hpp"

#include <GL/gl3w.h>

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

    m_textureMerger = &ResourceManager::getProgram("textureMerger");
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

    m_textureMerger->use();
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}