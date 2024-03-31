#include "TextureManager.hpp"

#include <GL/gl3w.h>

#include "Noise.hpp"
#include "Assert.hpp"
#include "Globals.hpp"

Texture& TextureManager::get(const std::string& name) {
    auto it = m_textures.find(name);
    massert(it != m_textures.end(), "Texture {} doesn't exist!", name);

    return it->second;
}

void TextureManager::resize(int size) {
    for (auto& [_, texture] : m_textures) {
        texture.setSize(size, size);
    }
}

Texture& TextureManager::resize(const std::string& name, int size) {
    auto& texture = this->get(name);
    texture.setSize(size, size);
    return texture;
}

Texture& TextureManager::insert(const std::string& name, int size, int binding, bool dualChannel) {
    auto [it, success] = m_textures.try_emplace(name);
    auto& texture      = it->second;

    auto format         = (dualChannel) ? GL_RG : GL_RGBA;
    auto internalFormat = (dualChannel) ? GL_RG32F : GL_RGBA32F;

    texture.setSize(size, size);
    texture.setFormat(internalFormat, format, GL_FLOAT);

    glActiveTexture(GL_TEXTURE0 + binding);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindImageTexture(binding, texture, 0, GL_FALSE, 0, GL_READ_WRITE, internalFormat);

    return texture;
}