#include "Program.hpp"
#include "Shader.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

Program::Program() { m_id = glCreateProgram(); }

uint32 Program::get() { return m_id; }

void Program::use() { glUseProgram(m_id); }

void Program::setUniform(const std::string& name, glm::mat4 mat) {
    auto location = glGetUniformLocation(m_id, name.c_str());
    if (location == -1) {
        std::cerr << "Uniform " << name << " not found in program.\n";
        // TODO: throw
        return;
    }

    glProgramUniformMatrix4fv(m_id, location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Program::attachShader(Shader& shader) {
    if (!m_linked) {
        shader.compile();
        glAttachShader(m_id, shader.get());
        return;
    }

    std::cerr << "Attach called on linked program.\n";
    // TODO: throw
}

void Program::link() {
    if (m_linked) {
        return;
    }

    glLinkProgram(m_id);

    int len;
    glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &len);
    if (len > 1) {
        char buffer[len + 1];
        glGetProgramInfoLog(m_id, len + 1, nullptr, buffer);
        std::cerr << buffer << '\n';
    }

    int success{};
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        std::cerr << "Error: Linking failed.\n";
        // TODO: throw
    }

    m_linked = true;
}