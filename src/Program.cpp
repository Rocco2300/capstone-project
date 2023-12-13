#include "Program.hpp"
#include "Shader.hpp"
#include "Assert.hpp"

#include <fmt/core.h>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <memory>
#include <sstream>

Program::Program() { m_id = glCreateProgram(); }

Program::~Program() { glDeleteProgram(m_id); }

uint32 Program::get() { return m_id; }

void Program::use() { glUseProgram(m_id); }

void Program::setUniform(const std::string& name, glm::mat4 mat) {
    auto location = glGetUniformLocation(m_id, name.c_str());
    massert(location != -1, "Uniform {} not found in program.\n", name);

    glProgramUniformMatrix4fv(m_id, location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Program::attachShader(Shader& shader) {
    massert(m_linked == false, "Attach called on linked program.\n");

    shader.compile();
    glAttachShader(m_id, shader.get());
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
    massert(success == true, "Error: Linking of program failed.\n");

    m_linked = true;
}