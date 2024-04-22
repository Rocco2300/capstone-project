#include "Plane.hpp"

Plane::Plane()
    : m_width{2}
    , m_height{2}
    , m_spacing{1.f} {}

Plane::Plane(uint32 width, uint32 height) {
    m_width   = width;
    m_height  = height;
    m_spacing = 1.f;

    generate(width, height);
}

glm::vec2 Plane::getSize() {
    return {m_width * m_spacing, m_height * m_spacing};
}

void Plane::setSpacing(float spacing) { m_spacing = spacing; }

void Plane::generate(uint32 width, uint32 height) {
    m_width  = width;
    m_height = height;

    std::vector<uint32> indices;
    std::vector<Vertex> vertices;
    for (size_t z = 0; z < m_height; z++) {
        for (size_t x = 0; x < m_width; x++) {
            Vertex vertex{x * m_spacing, 0.f, z * m_spacing};
            vertices.push_back(vertex);
        }
    }

    for (size_t j = 0; j < m_height - 1; j++) {
        for (size_t i = 0; i < m_width - 1; i++) {
            indices.push_back(indexFrom2D(i, j));
            indices.push_back(indexFrom2D(i + 1, j + 1));
            indices.push_back(indexFrom2D(i + 1, j));

            indices.push_back(indexFrom2D(i, j));
            indices.push_back(indexFrom2D(i, j + 1));
            indices.push_back(indexFrom2D(i + 1, j + 1));
        }
    }

    setData(indices, vertices);
}

uint32 Plane::indexFrom2D(int x, int y) const { return y * m_width + x; }