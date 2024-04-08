#include "Noise.hpp"

#include <random>

static float normalRandom() {
    static std::random_device rnd;
    static std::mt19937 gen(rnd());
    static std::normal_distribution<float> dist(-1.0f, 1.0f);

    return dist(gen);
}

Noise::Noise(uint16 width, uint16 height) {
    generateNoise(width, height);
}

void* Noise::data() { return &m_data[0]; }

uint16 Noise::width() { return m_width; }

uint16 Noise::height() { return m_height; }

void Noise::generateNoise(uint16 width, uint16 height) {
    m_width  = width;
    m_height = height;
    m_data.resize(m_width * m_height * 4);

    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            auto idx = (y * m_width + x) * 4;
            m_data[idx + 0] = (normalRandom() + 1.f) / 2.f;
            m_data[idx + 1] = (normalRandom() + 1.f) / 2.f;
            m_data[idx + 2] = 0.0f;
            m_data[idx + 3] = 1.0f;
        }
    }
}