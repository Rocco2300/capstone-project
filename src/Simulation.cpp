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
    m_size = size;

    std::vector<float> initialData(4 * m_size * m_size, 0);
    auto* texArray = &ResourceManager::getTexture("buffers");
    texArray->setData(&initialData[0], HEIGHT_INDEX);
    texArray->setData(&initialData[0], NORMAL_INDEX);
    texArray->setData(&initialData[0], DISPLACEMENT_INDEX);

    m_spacing = 64.0f / m_size;
    m_oceanPlane.setSpacing(m_spacing);
    m_oceanPlane.generate(m_size, m_size);
    m_oceanPlane.setOrigin({m_oceanPlane.getSize().x / 2.f, 0.f, m_oceanPlane.getSize().y / 2.f});
    m_oceanPlane.setPosition({0.f, -2.f, 0.f});

    m_textureMerger = &ResourceManager::getProgram("textureMerger");
}

float Simulation::getSpacing() { return m_spacing; }

glm::mat4& Simulation::getTransform() { return m_oceanPlane.getTransform(); }

SpectrumParameters& Simulation::params() { return m_spectrum.params(); }

void Simulation::toggleDebug() { m_debug = !m_debug; }

void Simulation::setSize(int size) {
    m_size = size;

    m_spacing = 64.0f / m_size;
    m_oceanPlane.setSpacing(m_spacing);
    m_oceanPlane.generate(m_size, m_size);

    ResourceManager::getProgram("ocean").setUniform("spacing", m_spacing);
    ResourceManager::getProgram("debugNormals").setUniform("spacing", m_spacing);

    ResourceManager::resizeImages(m_size);
    ResourceManager::resizeTextures(m_size);

    std::vector<float> initialData(4 * m_size * m_size, 0);
    auto* texArray = &ResourceManager::getTexture("buffers");
    texArray->setData(&initialData[0], HEIGHT_INDEX);
    texArray->setData(&initialData[0], NORMAL_INDEX);
    texArray->setData(&initialData[0], DISPLACEMENT_INDEX);

    m_dft.setSize(m_size);
    m_fft.setSize(m_size);
    m_spectrum.setSize(m_size);
}

void Simulation::setAlgorithm(Algorithm algorithm) {
    m_algorithm = algorithm;

    // prevent using displacement when changing from gerstner to sines
    if (algorithm == Algorithm::Sines) {
        std::vector<float> initialData(4 * m_size * m_size, 0);
        auto* texArray = &ResourceManager::getTexture("buffers");
        texArray->setData(&initialData[0], HEIGHT_INDEX);
        texArray->setData(&initialData[0], NORMAL_INDEX);
        texArray->setData(&initialData[0], DISPLACEMENT_INDEX);
    }

    if (algorithm == Algorithm::SlowGerstner || algorithm == Algorithm::SlowFFT) {
        m_spectrum.setAccelerated(false);
    } else {
        m_spectrum.setAccelerated(true);
    }

    m_spectrum.initialize();
}

void Simulation::initialize() { m_spectrum.initialize(); }

void Simulation::update(float time) {
    m_spectrum.update(time);

    bool displaceNormals = true;
    switch (m_algorithm) {
    case Algorithm::Sines:
        m_dft.dispatchSines();
        displaceNormals = false;
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
        m_fft.dispatchIFFTGPU(DY_INDEX, HEIGHT_INDEX);
        m_fft.dispatchIFFTGPU(DX_DZ_INDEX, DISPLACEMENT_INDEX);
        m_fft.dispatchIFFTGPU(DYX_DYZ_INDEX, NORMAL_INDEX);
        break;
    case Algorithm::SlowGerstner:
        m_dft.dispatchGerstnerCPU();
        break;
    case Algorithm::SlowFFT:
        m_fft.dispatchIFFTCPU("dy", HEIGHT_INDEX);
        m_fft.dispatchIFFTCPU("dx_dz", DISPLACEMENT_INDEX);
        m_fft.dispatchIFFTCPU("dyx_dyz", NORMAL_INDEX);
        break;
    }

    m_textureMerger->use();
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Simulation::draw() {
    m_oceanPlane.bind();
    ResourceManager::getProgram("ocean").use();
    glDrawElements(GL_TRIANGLES, m_oceanPlane.getIndices().size(), GL_UNSIGNED_INT, 0);

    if (m_debug) {
        ResourceManager::getProgram("debugNormals").use();
        glDrawElements(GL_TRIANGLES, m_oceanPlane.getIndices().size(), GL_UNSIGNED_INT, 0);
    }
    m_oceanPlane.unbind();
}