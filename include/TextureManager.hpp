#pragma once

#include "Texture.hpp"

#include <string>
#include <unordered_map>

class TextureManager {
private:
    std::unordered_map<std::string, Texture> m_textures;

public:
    TextureManager() = default;

    Texture& get(const std::string& name);

    void resize(int size);
    Texture& resize(const std::string& name, int size);
    Texture& insert(const std::string& name, int size, int binding, bool dualChannel = false);
};