#pragma once

#include "Plane.hpp"
#include "Program.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

struct alignas(8) Wave {
    glm::vec2 direction;
    float amplitude;
    float frequency;
    float phase;
};

class Simulation {
protected:
    uint32 m_width;
    uint32 m_height;

    Plane* m_surface;
    Texture* m_normal;
    Texture* m_displacement;

public:
    virtual void init(uint32 width, uint32 height) = 0;
    virtual void update(double time)               = 0;

    void resize(int width, int height);

    void setNormal(Texture& normal);
    void setSurface(Plane& surface);
    void setDisplacement(Texture& displacement);
};

class SineSimulation : public Simulation {
private:
    Program m_updateProgram;

    uint32 m_wavesSSBO{};

public:
    void init(uint32 width, uint32 height) override;
    void update(double time) override;

    void generateWaves(uint32 count);
};