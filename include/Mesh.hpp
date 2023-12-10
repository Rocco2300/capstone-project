#pragma once

#include "Transformable.hpp"
#include "Types.hpp"

#include <vector>

struct Vertex {
    float x, y, z;
};

class Mesh : public Transformable {
private:
    uint32 m_vao{};
    uint32 m_vbo{};
    uint32 m_ebo{};

    std::vector<Vertex> m_vertices;
    std::vector<uint32> m_indices;

public:
    Mesh() = default;
    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<uint32>& indices);

    std::vector<uint32>& getIndices();

    void bind();
    void unbind();

private:
    void bindBufferData();
};