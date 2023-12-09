#pragma once

#include "Types.hpp"

#include <GL/glew.h>

#include <string>

class Shader {
private:
    uint32 m_id;

public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    void bind();

private:
    std::string loadFile(const std::string& path);
    uint32 compileShader(const char* shaderCode, GLenum shaderType);
    void linkProgram(uint32 vertexShaderId, uint32 fragmentShaderId);
};