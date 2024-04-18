#pragma once

#include "DFT.hpp"
#include "FFT.hpp"
#include "Spectrum.hpp"
#include "TextureManager.hpp"

enum class Algorithm {
    Sines,
    Gerstner,
    DFT,
    FFT
};

class Simulation {
private:
    int m_size{};

    Algorithm m_algorithm{};
    TextureManager* m_textureManager{};

    DFT m_dft;
    FFT m_fft;

    Spectrum m_spectrum;
    Program m_textureMerger;

public:
    Simulation(TextureManager& textureManager, int size);

    void setAlgorithm(Algorithm algorithm);

    void update(float time);
};