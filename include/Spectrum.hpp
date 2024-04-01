#pragma once

#include "Program.hpp"

struct SpectrumParameters
{
    float scale;
    float angle;
    float depth;
    float fetch;
    float gamma;
    float swell;
    float windSpeed;
    float spreadBlend;
};

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