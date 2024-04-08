#include "Shader.hpp"
#include "Assert.hpp"

#include <GL/gl3w.h>

#include <fstream>
#include <iostream>
#include <sstream>

static int shaderTypetoGLenum(Shader::Type type) {
    switch (type) {
    case Shader::Type::Vertex:
        return GL_VERTEX_SHADER;
    case Shader::Type::Fragment:
        return GL_FRAGMENT_SHADER;
    case Shader::Type::Compute:
        return GL_COMPUTE_SHADER;
    default:
        return -1;
    }
}

static std::string shaderTypeToString(Shader::Type type) {
    switch (type) {
    case Shader::Type::Vertex:
        return "vertex";
    case Shader::Type::Fragment:
        return "fragment";
    case Shader::Type::Compute:
        return "compute";
    default:
        return "none";
    }
}

Shader::Shader(Shader::Type type)
    : m_type{type} {}

Shader::Shader(Shader::Type type, const std::string& filePath)
    : m_type{type}
    , m_filePath{filePath} {
    load(m_filePath);
}

Shader::~Shader() { glDeleteShader(m_id); }

uint32 Shader::get() { return m_id; }

void Shader::compile() {
    if (m_compiled) {
        return;
    }

    auto* code = m_sourceCode.c_str();
    m_id       = glCreateShader(shaderTypetoGLenum(m_type));
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
    massert(success == true, "Error: Compilation of {} {} failed.\n", shaderTypeToString(m_type),
            m_filePath);

    m_compiled = true;
}

void Shader::load(const std::string& filePath) {
    m_filePath = filePath;

    std::ifstream in(filePath);

    if (in.fail()) {
        std::cerr << "Error: Cannot open file " << filePath << '\n';
        return;
    }

    if (m_sourceCode.empty()) {
        m_sourceCode += "#version 460\n";
    }

    std::stringstream fileContent;
    fileContent << in.rdbuf();
    auto code  = fileContent.str();
    auto index = code.find("#include");
    if (index != std::string::npos) {
        auto start = code.find('<') + 1;
        auto size  = code.find('>') - start;
        auto name  = code.substr(code.find('<') + 1, size);
        std::ifstream headerStream("../include/" + name);
        std::stringstream headerContent;
        headerContent << headerStream.rdbuf();

        auto header      = headerContent.str();
        auto pragmaIndex = header.find("#pragma");

        header.erase(pragmaIndex, header.find('\n', index) + 1);
        code.erase(index, code.find('\n', index) + 1);

        m_sourceCode += header;
    }

    m_sourceCode += code;
    m_sourceCode += '\n';
}
