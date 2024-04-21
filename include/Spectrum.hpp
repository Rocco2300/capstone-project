#pragma once

#include "Image.hpp"

#include <glm/glm.hpp>

class Program;

struct JonswapParameters
{
    float scale;
    float angle;
    float depth;
    float fetch;
    float gamma;
    float patchSize;
    float windSpeed;
    float spreadBlend;
};

struct PhillipsParameters {
    float A;
    float patchSize;
    glm::vec2 wind;
};

typedef PhillipsParameters SpectrumParameters;

class Spectrum {
private:
    int m_size{};
    bool m_accelerated{};

    uint32 m_paramsSSBO;
    SpectrumParameters m_params;

    Program* m_initialProgram;
    Program* m_timeDependentProgram;

    NoiseImage m_noiseImage;

public:
    explicit Spectrum(int size);

    SpectrumParameters& params();

    void setSize(int size);
    void setAccelerated(bool accelerated);
    void setParameters(SpectrumParameters& params);

    void initialize();
    void update(float time);

private:
    float dispersion(float k);
    float phillips(glm::vec2 k);
    void computeInitialSpectrum();
    void evolveCPUSpectrum(float time);
    void evolveGPUSpectrum(float time);
};