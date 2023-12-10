#pragma once

#include "Mesh.hpp"

class Plane : public Mesh {
private:
    uint32 m_width;
    uint32 m_height;

public:
    Plane() = default;
    Plane(uint32 width, uint32 height);

    void generate(uint32 width, uint32 height);

private:
    uint32 indexFrom2D(int x, int y);
};