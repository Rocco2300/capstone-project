#include "Mesh.hpp"

#include <GL/gl3w.h>
#include <glm/gtx/transform.hpp>

Mesh::Mesh(const std::vector<uint32>& indices, const std::vector<Vertex>& vertices) {
    setData(indices, vertices);
}

std::vector<uint32>& Mesh::getIndices() { return m_indices; }

void Mesh::setData(const std::vector<uint32>& indices, const std::vector<Vertex>& vertices) {
    m_indices  = indices;
    m_vertices = vertices;

    bindBufferData();
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
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0],
                 GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32), &m_indices[0],
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}