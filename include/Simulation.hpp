#pragma once

#include "Texture.hpp"

struct Params
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

class Simulation {
private:
    Texture h0;
    Texture h0K;
    Texture noise;

public:
    Simulation() = default;

    void setInitializationCompute(std::string_view path);

    void init();
};