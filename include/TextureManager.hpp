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

    void resize(int size, int depth = 1);
    Texture& resize(const std::string& name, int size, int depth = 1);
    Texture& insert(const std::string& name,
                    int binding,
                    int size,
                    int depth        = 1,
                    bool dualChannel = false);
};