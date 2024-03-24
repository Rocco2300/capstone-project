#pragma once

#include "Types.hpp"

#include <vector>

class Noise {
private:
    uint16 m_width;
    uint16 m_height;

    std::vector<float> m_data;

public:
    Noise() = default;
    Noise(uint16 width, uint16 height);

    void* data();
    uint16 width();
    uint16 height();

    void generateNoise(uint16 width, uint16 height);

private:
};