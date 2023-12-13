#include "Shader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

static std::string shaderTypeToString(Shader::Type type) {
    switch (type) {
    case Shader::Type::Vertex:
        return "vertex";
    case Shader::Type::Fragment:
        return "fragment";
    case Shader::Type::Compute:
        return "compute";
    default:
        return "";
    }
}

Shader::Shader(Shader::Type type)
    : m_type{type} {}

Shader::Shader(Shader::Type type, const std::string& filePath)
    : m_type{type}
    , m_filePath{filePath} {
    load(m_filePath);
}

uint32 Shader::get() { return m_id; }

void Shader::compile() {
    if (m_compiled) {
        return;
    }

    auto* code = m_sourceCode.c_str();
    m_id       = glCreateShader(static_cast<GLenum>(m_type));
    glShaderSource(m_id, 1, &code, nullptr);
    glCompileShader(m_id);

    int len{};
    glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &len);
    if (len > 1) {
        char buffer[len + 1];
        glGetShaderInfoLog(m_id, len + 1, nullptr, buffer);
        std::cerr << buffer << '\n';
    }

    int success{};
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::cerr << "Error: Compilation of " << shaderTypeToString(m_type) << ' ' << m_filePath
                  << ' ' << " failed.\n";

        glDeleteShader(m_id);
        m_compiled = false;
        // TODO: throw
        return;
    }

    m_compiled = true;
}

void Shader::load(const std::string& filePath) {
    std::ifstream in(filePath);
    if (in.fail()) {
        std::cout << "Error: Cannot open file " << filePath << '\n';
        // TODO: throw
        return;
    }

    std::stringstream ss;
    ss << in.rdbuf();
    m_sourceCode = ss.str();
}