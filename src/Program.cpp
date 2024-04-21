#include "Program.hpp"
#include "Shader.hpp"
#include "Assert.hpp"

#include <fmt/core.h>
#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <memory>
#include <sstream>

static void printInfoLog(uint32 id) {
    int len;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
    if (len > 1) {
        char buffer[len + 1];
        glGetProgramInfoLog(id, len + 1, nullptr, buffer);
        std::cerr << buffer << '\n';
    }
}

Program::Program() { m_id = glCreateProgram(); }

Program::~Program() { glDeleteProgram(m_id); }

uint32 Program::get() { return m_id; }

void Program::use() { glUseProgram(m_id); }

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

    printInfoLog(m_id);

    int success{};
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    massert(success == true, "Error: Linking of program failed.\n");

    m_linked = true;
}

void Program::validate() {
    if (!m_linked) {
        link();
    }

    glValidateProgram(m_id);

    printInfoLog(m_id);

    int success{};
    glGetProgramiv(m_id, GL_VALIDATE_STATUS, &success);
    massert(success == true, "Error: Validation of program failed.\n");
}

void Program::setUniform(std::string_view name, int value) {
    auto location = glGetUniformLocation(m_id, name.data());
    massert(location != -1, "Uniform {} not found in program.\n", name);

    glProgramUniform1i(m_id, location, value);
}

void Program::setUniform(std::string_view name, double value) {
    auto location = glGetUniformLocation(m_id, name.data());
    massert(location != -1, "Uniform {} not found in program.\n", name);

    glProgramUniform1d(m_id, location, value);
}

void Program::setUniform(std::string_view name, glm::vec4 value) {
    auto location = glGetUniformLocation(m_id, name.data());
    massert(location != -1, "Uniform {} not found in program.\n", name);

    glProgramUniform4fv(m_id, location, 1, glm::value_ptr(value));
}

void Program::setUniform(std::string_view name, glm::mat4 value) {
    auto location = glGetUniformLocation(m_id, name.data());
    massert(location != -1, "Uniform {} not found in program.\n", name);

    glProgramUniformMatrix4fv(m_id, location, 1, GL_FALSE, glm::value_ptr(value));
}