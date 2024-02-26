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

    void setInt(std::string_view name, int value);
    void setFloat(std::string_view name, float value);
    void setDouble(std::string_view name, double value);
    void setMatrix4(std::string_view name, glm::mat4 value);
};