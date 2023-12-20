#pragma once

#include "Types.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <cassert>

class Shader;

class Program {
private:
    uint32 m_id{};

    bool m_linked{};

public:
    Program();
    ~Program();

    uint32 get();

    void use();
    void link();
    void validate();
    void attachShader(Shader& shader);

    void setUniform(std::string_view name, int value);
    void setUniform(std::string_view name, glm::mat4 value);
};