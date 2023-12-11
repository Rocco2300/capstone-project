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
    Mesh(const std::vector<uint32>& indices,
         const std::vector<Vertex>& vertices);

    std::vector<uint32>& getIndices();

    void setData(const std::vector<uint32>& indices,
                 const std::vector<Vertex>& vertices);

    void bind();
    void unbind();

private:
    void bindBufferData();
};