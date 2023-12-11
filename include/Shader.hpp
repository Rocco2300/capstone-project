#pragma once

#include "Types.hpp"

#include <string>

class Shader {
private:
    uint32 m_id;

public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    uint32 getId();

    void use();

private:
    std::string loadFile(const std::string& path);
    uint32 compileShader(const char* shaderCode, uint32 shaderType);
    void linkProgram(uint32 vertexShaderId, uint32 fragmentShaderId);
};