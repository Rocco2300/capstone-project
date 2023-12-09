#include "Mesh.hpp"

#include <GL/glew.h>
#include <glm/gtx/transform.hpp>

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<uint32>& indices)
    : m_vertices{vertices}, m_indices{indices} {
    bindBufferData();

    m_position = glm::vec3(0.f, 0.f, 0.f);
    m_rotation = glm::vec3(0.f, 0.f, 0.f);
    m_scale = glm::vec3(1.f, 1.f, 1.f);

    m_transform = glm::mat4(1.f);
}

glm::mat4 Mesh::getTransform() {
    if (m_transformNeedUpdate) {
        updateTransform();
    }

    return m_transform;
}

std::vector<uint32>& Mesh::getIndices() { return m_indices; }

void Mesh::setScale(glm::vec3 scale) {
    m_scale = scale;
    m_transformNeedUpdate = true;
}

void Mesh::setPosition(glm::vec3 position) {
    m_position = position;
    m_transformNeedUpdate = true;
}

void Mesh::setRotation(glm::vec3 rotation) {
    m_rotation = rotation;
    m_transformNeedUpdate = true;
}

void Mesh::bind() {
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
}

void Mesh::unbind() {
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::bindBufferData() {
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
                 &m_vertices[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32),
                 &m_indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::updateTransform() {
    m_transform = glm::mat4(1.f);

    auto scale = glm::mat4(1.f);
    auto rotation = glm::mat4(1.f);
    auto translate = glm::mat4(1.f);

    scale = glm::scale(scale, m_scale);
    rotation = glm::rotate(rotation, m_rotation.x, glm::vec3(1.f, 0.f, 0.f));
    translate = glm::translate(translate, m_position);

    m_transform = m_transform * translate * scale * rotation;
    m_transformNeedUpdate = false;
}