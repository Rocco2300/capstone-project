#include "Spectrum.hpp"

#include "Globals.hpp"
#include "Profiler.hpp"
#include "ResourceManager.hpp"

#include "GL/gl3w.h"

#include <glm/gtc/constants.hpp>

const float g = 9.81;

Spectrum::Spectrum(int size) {
    m_size = size;

    auto windSpeed     = 25.0f;
    auto windDirection = glm::pi<float>() / 4.f;
    auto wind          = glm::vec2(glm::cos(windDirection), glm::sin(windDirection)) * windSpeed;
    m_params.A         = 4.0f;
    m_params.patchSize = 1750.0f;
    m_params.lowCutoff = 0.01f;
    m_params.wind      = wind;

    glGenBuffers(1, &m_paramsSSBO);

    m_noiseImage           = NoiseImage(m_size, m_size);
    m_initialProgram       = &ResourceManager::getProgram("initialSpectrum");
    m_timeDependentProgram = &ResourceManager::getProgram("timeDependentSpectrum");
    m_initialProgram->setUniform("size", m_size);
}

SpectrumParameters& Spectrum::params() { return m_params; }

void Spectrum::setSize(int size) {
    m_size = size;
    m_initialProgram->setUniform("size", m_size);
    m_noiseImage = NoiseImage(m_size, m_size);
}

void Spectrum::setAccelerated(bool accelerated) { m_accelerated = accelerated; }

void Spectrum::initialize() {
    if (m_accelerated) {
        computeInitialGPUSpectrum();
    } else {
        computeInitialCPUSpectrum();
    }
}

void Spectrum::update(float time) {
    if (m_accelerated) {
        evolveGPUSpectrum(time);
    } else {
        evolveCPUSpectrum(time);
    }
}

float Spectrum::dispersion(float k) { return glm::sqrt(g * k); }

float Spectrum::phillips(glm::vec2 k) {
    float kLength = glm::length(k);
    if (kLength < 0.0001) {
        return 0.0;
    }

    float windSpeed = glm::length(m_params.wind);
    float L         = windSpeed * windSpeed / g;
    float kL        = kLength * L;
    float kw        = dot(normalize(k), normalize(m_params.wind));

    float damping = 0.001;
    float l       = L * damping;

    return m_params.A * exp(-1.0 / (kL * kL)) / (kLength * kLength * kLength * kLength) *
           (kw * kw) * glm::exp(-kLength * kLength * l * l);
}

void Spectrum::computeInitialGPUSpectrum() {
    Profiler::functionBegin("GenerateOceanSpectrum");

    auto* texArray = &ResourceManager::getTexture("buffers");

    Profiler::queryBegin();
    texArray->setData(m_noiseImage.data(), NOISE_INDEX);
    Profiler::queryEnd();

    Profiler::queryBegin();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_paramsSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(m_params), &m_params, GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, PARAMS_BINDING, m_paramsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    Profiler::queryEnd();

    Profiler::queryBegin();
    m_initialProgram->use();
    m_initialProgram->setUniform("conjugate", 0);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    Profiler::queryEnd();

    Profiler::queryBegin();
    m_initialProgram->setUniform("conjugate", 1);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    Profiler::queryEnd();

    Profiler::functionEnd("GenerateOceanSpectrum");
}

void Spectrum::computeInitialCPUSpectrum() {
    Profiler::functionBegin("GenerateOceanSpectrum");

    Image temp(m_size, m_size);
    auto& wavedata        = ResourceManager::getImage("wavedata");
    auto& initialSpectrum = ResourceManager::getImage("initialSpectrum");
    for (int y = 0; y < m_size; y++) {
        for (int x = 0; x < m_size; x++) {
            float deltaK  = 2.0f * glm::pi<float>() / m_params.patchSize;
            int nx        = int(x - m_size / 2);
            int ny        = int(y - m_size / 2);
            glm::vec2 k   = (glm::vec2(nx, ny) * deltaK) + glm::vec2(0.00001);
            float kLength = glm::length(k);

            float spectrum   = phillips(k);
            float omega      = dispersion(kLength);
            auto& noiseValue = m_noiseImage.at(x, y);

            auto& wave   = wavedata.at(x, y);
            auto& hTilde = temp.at(x, y);
            wave         = {k.x, 1 / kLength, k.y, omega};
            hTilde       = {noiseValue.r * glm::sqrt(spectrum / 2.0f),
                            noiseValue.g * glm::sqrt(spectrum / 2.0f), 0.0f, 1.0f};
        }
    }

    for (int y = 0; y < m_size; y++) {
        for (int x = 0; x < m_size; x++) {
            auto& h0K      = temp.at(x, y);
            auto& h0MinusK = temp.at((m_size - x) % m_size, (m_size - y) % m_size);
            auto& hTilde   = initialSpectrum.at(x, y);
            hTilde         = {h0K.r, h0K.g, h0MinusK.r, -h0MinusK.g};
        }
    }

    Profiler::functionEnd("GenerateOceanSpectrum");
}

static glm::vec2 complexMul(glm::vec2 a, glm::vec2 b) {
    return glm::vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

void Spectrum::evolveCPUSpectrum(float time) {
    Profiler::functionBegin("EvolveOceanSpectrum");
    auto& wavedata        = ResourceManager::getImage("wavedata");
    auto& initialSpectrum = ResourceManager::getImage("initialSpectrum");

    auto& dyImage      = ResourceManager::getImage("dy");
    auto& dx_dzImage   = ResourceManager::getImage("dx_dz");
    auto& dyx_dyzImage = ResourceManager::getImage("dyx_dyz");
    for (int y = 0; y < m_size; y++) {
        for (int x = 0; x < m_size; x++) {
            auto& wave    = wavedata.at(x, y);
            auto& initial = initialSpectrum.at(x, y);

            auto& dy      = dyImage.at(x, y);
            auto& dx_dz   = dx_dzImage.at(x, y);
            auto& dyx_dyz = dyx_dyzImage.at(x, y);

            float phase = wave.a * time;
            glm::vec2 exponent(glm::cos(phase), glm::sin(phase));
            glm::vec2 exponentConj(exponent.x, -exponent.y);

            glm::vec2 h0(initial.r, initial.g);
            glm::vec2 h0Conj(initial.b, initial.a);
            glm::vec2 h = complexMul(h0, exponent) + complexMul(h0Conj, exponentConj);
            dy.r        = h.x;
            dy.g        = h.y;

            glm::vec2 ih(-h.y, h.x);
            glm::vec2 n = complexMul(ih, glm::vec2(wave.r, wave.b));
            glm::vec2 d = complexMul(-ih, glm::vec2(wave.r, wave.b) * wave.g);
            dx_dz.r     = d.x;
            dx_dz.g     = d.y;
            dyx_dyz.r   = n.x;
            dyx_dyz.g   = n.y;
        }
    }
    Profiler::functionEnd("EvolveOceanSpectrum");
}

void Spectrum::evolveGPUSpectrum(float time) {
    Profiler::functionBegin("EvolveOceanSpectrum");

    Profiler::queryBegin();
    m_timeDependentProgram->setUniform("time", time);
    m_timeDependentProgram->use();
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    Profiler::queryEnd();

    Profiler::functionEnd("EvolveOceanSpectrum");
}
