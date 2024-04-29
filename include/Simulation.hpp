#pragma once

#include "DFT.hpp"
#include "FFT.hpp"
#include "Plane.hpp"
#include "Spectrum.hpp"

class Program;

enum class Algorithm : int {
    Sines = 0,
    Gerstner,
    DFT,
    FFT,
    SlowGerstner,
    SlowFFT
};

class Simulation {
private:
    int m_size{};
    bool m_debug{};
    float m_spacing{};

    DFT m_dft;
    FFT m_fft;

    Plane m_oceanPlane;
    Spectrum m_spectrum;
    Program* m_textureMerger;

    Algorithm m_algorithm{};

public:
    Simulation(int size);

    float getSpacing();
    glm::mat4& getTransform();
    SpectrumParameters& params();

    void toggleDebug();
    void setSize(int size);
    void setAlgorithm(Algorithm algorithm);

    void initialize();
    void update(float time);
    void draw();
};