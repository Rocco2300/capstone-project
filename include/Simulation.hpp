#pragma once

#include "DFT.hpp"
#include "FFT.hpp"
#include "ResourceManager.hpp"
#include "Spectrum.hpp"

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

    DFT m_dft;
    FFT m_fft;

    Spectrum m_spectrum;
    Program m_textureMerger;

    Algorithm m_algorithm{};

public:
    Simulation(int size);

    void setAlgorithm(Algorithm algorithm);

    void update(float time);
};