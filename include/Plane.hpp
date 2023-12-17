#pragma once

#include "Mesh.hpp"

class Plane : public Mesh {
private:
    uint32 m_width{};
    uint32 m_height{};

    float m_spacing{};
public:
    Plane();
    // Create a plane with width * height vertices
    Plane(uint32 width, uint32 height);

    glm::vec2 getSize();

    void setSpacing(float spacing);
    // Generate a plane with width * height vertices
    void generate(uint32 width, uint32 height);

private:
    uint32 indexFrom2D(int x, int y) const;
};