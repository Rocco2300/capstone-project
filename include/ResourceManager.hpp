#pragma once

#include "Image.hpp"
#include "Texture.hpp"

#include <string>
#include <unordered_map>

class ResourceManager {
private:
    static std::unordered_map<std::string, Image> m_images;
    static std::unordered_map<std::string, Texture> m_textures;

public:
    static Image& getImage(const std::string& name);
    static Texture& getTexture(const std::string& name);

    static void resize(int size, int depth = 1);
    static Texture& resize(const std::string& name, int size, int depth = 1);
    static Image& insertImage(const std::string& name, int size);
    static Texture& insertTexture(const std::string& name,
                                  int binding,
                                  int size,
                                  int depth        = 1,
                                  bool dualChannel = false);
};