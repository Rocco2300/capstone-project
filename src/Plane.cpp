#include "Plane.hpp"

Plane::Plane(uint32 width, uint32 height) {
    m_width = width;
    m_height = height;
    generate(width, height);
}

uint32 Plane::indexFrom2D(int x, int y) { return y * m_width + x; }

void Plane::generate(uint32 width, uint32 height) {
    m_width = width;
    m_height = height;

    std::vector<uint32> indices;
    std::vector<Vertex> vertices;
    for (int y = 0; y <= m_height; y++) {
        for (int x = 0; x <= m_width; x++) {
            Vertex vertex{x * 0.5f, y * 0.5f, 0.0f};
            vertices.push_back(vertex);
        }
    }

    for (int y = 0; y <= m_height; y++) {
        for (int x = 0; x <= m_width; x++) {
            indices.push_back(indexFrom2D(x, y));
            indices.push_back(indexFrom2D(x + 1, y));
            indices.push_back(indexFrom2D(x + 1, y + 1));

            indices.push_back(indexFrom2D(x, y));
            indices.push_back(indexFrom2D(x + 1, y + 1));
            indices.push_back(indexFrom2D(x, y + 1));
        }
    }

    setData(indices, vertices);
}