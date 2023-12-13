#pragma once

#include "Program.hpp"
#include "Types.hpp"

#include <GL/glew.h>

#include <string>

class Shader {
public:
    friend class Program;

    enum class Type : uint32 {
        Vertex   = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Compute  = GL_COMPUTE_SHADER
    };

private:
    Type m_type;
    uint32 m_id;

    bool m_compiled{};

    std::string m_filePath;
    std::string m_sourceCode;

protected:
    uint32 get();

public:
    explicit Shader(Type type);
    Shader(Type type, const std::string& filePath);

    void compile();
    void load(const std::string& filePath);
};

class VertexShader : public Shader {
public:
    VertexShader()
        : Shader(Shader::Type::Vertex) {}
    explicit VertexShader(const std::string& filePath)
        : Shader(Shader::Type::Vertex, filePath) {}
};

class FragmentShader : public Shader {
public:
    FragmentShader()
        : Shader(Shader::Type::Fragment) {}
    explicit FragmentShader(const std::string& filePath)
        : Shader(Shader::Type::Fragment, filePath) {}
};

class ComputeShader : public Shader {
public:
    ComputeShader()
        : Shader(Shader::Type::Compute) {}
    explicit ComputeShader(const std::string& filePath)
        : Shader(Shader::Type::Compute, filePath) {}
};