#pragma once

#include "Types.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>

class Shader;

class Program {
private:
    uint32 m_id{};
    std::vector<Shader*> m_shaders;

    bool m_linked{};

public:
    Program();

    uint32 get();

    void use();
    void link();
    void validate();
    void attachShader(Shader& shader);
    void setUniform(const std::string& name, glm::mat4 mat);
};