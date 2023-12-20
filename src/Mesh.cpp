#include "Mesh.hpp"

#include <GL/gl3w.h>
#include <glm/gtx/transform.hpp>

Mesh::Mesh(const std::vector<uint32>& indices, const std::vector<Vertex>& vertices) {
    setData(indices, vertices);
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
}

std::vector<uint32>& Mesh::getIndices() { return m_indices; }

void Mesh::setData(const std::vector<uint32>& indices, const std::vector<Vertex>& vertices) {
    m_indices  = indices;
    m_vertices = vertices;

    bindBufferData();
}

void Mesh::bind() {
    glBindVertexArray(m_vao);
}

void Mesh::unbind() {
    glBindVertexArray(0);
}

void Mesh::bindBufferData() {
    if (!m_generated) {
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);
        glGenVertexArrays(1, &m_vao);

        m_generated = true;
    }

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0],
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32), &m_indices[0],
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}