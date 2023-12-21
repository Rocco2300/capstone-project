#pragma once

#include "Plane.hpp"
#include "Shader.hpp"
#include "Program.hpp"
#include "Texture.hpp"

class Simulation {
protected:
    Plane* m_surface;

    Texture* m_normal;
    Texture* m_displacement;

public:
    virtual void init()   = 0;
    virtual void update() = 0;

    void resize(int width, int height);

    void setNormal(Texture& normal);
    void setSurface(Plane& surface);
    void setDisplacement(Texture& displacement);
};

class SineSimulation : public Simulation {
private:
    Program m_updateProgram;

public:
    void init() override;
    void update() override;
};