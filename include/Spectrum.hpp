#pragma once

#include "Program.hpp"

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
    float a;
    float patchSize;
    glm::vec2 wind;
};

typedef PhillipsParameters SpectrumParameters;

class Spectrum {
private:
    int m_size;
    uint32 m_paramsSSBO;
    SpectrumParameters m_params;

    Program m_initialProgram;
    Program m_timeDependentProgram;

public:
    Spectrum(int size, const SpectrumParameters& params);

    void initialize();
    void update(double time);
};