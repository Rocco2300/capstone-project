#pragma once

#include "Texture.hpp"

#include <string>
#include <unordered_map>

class TextureManager {
private:
    static std::unordered_map<std::string, Texture> m_textures;

public:
    static Texture& get(const std::string& name);

    static void resize(int size, int depth = 1);
    static Texture& resize(const std::string& name, int size, int depth = 1);
    static Texture& insert(const std::string& name,
                           int binding,
                           int size,
                           int depth        = 1,
                           bool dualChannel = false);
};