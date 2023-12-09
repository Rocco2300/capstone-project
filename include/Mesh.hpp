#pragma once

#include "Types.hpp"

#include <glm/glm.hpp>

#include <vector>

struct Vertex {
    float x, y, z;
};

class Mesh {
private:
    uint32 m_vao{};
    uint32 m_vbo{};
    uint32 m_ebo{};

    glm::vec3 m_position{};
    glm::vec3 m_rotation{};
    glm::vec3 m_scale{};
    glm::vec2 m_size{};

    glm::mat4 m_transform{};
    bool m_transformNeedUpdate{};

    std::vector<Vertex> m_vertices;
    std::vector<uint32> m_indices;

public:
    Mesh() = default;
    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<uint32>& indices);

    glm::mat4 getTransform();
    std::vector<uint32>& getIndices();

    void setScale(glm::vec3 scale);
    void setPosition(glm::vec3 position);
    void setRotation(glm::vec3 rotation);

    void bind();
    void unbind();

private:
    void bindBufferData();
    void updateTransform();
};