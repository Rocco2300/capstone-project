#include "TextureManager.hpp"

#include <GL/gl3w.h>

#include "Assert.hpp"
#include "Globals.hpp"
#include "Noise.hpp"

Texture& TextureManager::get(const std::string& name) {
    auto it = m_textures.find(name);
    massert(it != m_textures.end(), "Texture {} doesn't exist!", name);

    return it->second;
}

void TextureManager::resize(int size, int depth) {
    for (auto& [_, texture]: m_textures) { texture.setSize(size, size, depth); }
}

Texture& TextureManager::resize(const std::string& name, int size, int depth) {
    auto& texture = this->get(name);
    texture.setSize(size, size, depth);
    return texture;
}

Texture& TextureManager::insert(const std::string& name,
                                int binding,
                                int size,
                                int depth,
                                bool dualChannel) {
    auto [it, success] = m_textures.try_emplace(name);
    auto& texture      = it->second;

    auto format         = (dualChannel) ? GL_RG : GL_RGBA;
    auto internalFormat = (dualChannel) ? GL_RG32F : GL_RGBA32F;

    texture.setSize(size, size, depth);
    texture.setFormat(internalFormat, format, GL_FLOAT);
    texture.create();

    int layered = (depth > 1) ? GL_TRUE : GL_FALSE;
    glActiveTexture(GL_TEXTURE0 + binding);
    glBindTexture(texture.target(), texture);
    glBindImageTexture(binding, texture, 0, layered, 0, GL_READ_WRITE, internalFormat);
    glBindTexture(texture.target(), 0);

    return texture;
}