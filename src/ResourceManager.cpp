#include "ResourceManager.hpp"

#include <GL/gl3w.h>

#include "Assert.hpp"
#include "Shader.hpp"
#include "Globals.hpp"
#include "Image.hpp"

std::unordered_map<std::string, Image> ResourceManager::m_images{};
std::unordered_map<std::string, Program> ResourceManager::m_programs{};
std::unordered_map<std::string, Texture> ResourceManager::m_textures{};

Image& ResourceManager::getImage(const std::string& name) {
    auto it = m_images.find(name);
    massert(it != m_images.end(), "Image {} doesn't exist!", name);

    return it->second;
}

Texture& ResourceManager::getTexture(const std::string& name) {
    auto it = m_textures.find(name);
    massert(it != m_textures.end(), "Texture {} doesn't exist!", name);

    return it->second;
}

void ResourceManager::resize(int size, int depth) {
    for (auto& [_, texture]: m_textures) { texture.setSize(size, size, depth); }
}

Texture& ResourceManager::resize(const std::string& name, int size, int depth) {
    auto& texture = ResourceManager::getTexture(name);
    texture.setSize(size, size, depth);
    return texture;
}

Image& ResourceManager::insertImage(const std::string& name, int size) {
    massert(m_images.find(name) == m_images.end(), "Cannot insert duplicate image {}", name);

    auto [it, success] = m_images.try_emplace(name);
    auto& image        = it->second;

    image = Image(size, size);
    return image;
}

Program& ResourceManager::insertProgram(const std::string& name, const std::string& path) {
    massert(m_programs.find(name) == m_programs.end(), "Cannot insert duplicate program {}", name);

    auto [it, success] = m_programs.try_emplace(name);
    auto& program      = it->second;

    ComputeShader shader(path);
    program.attachShader(shader);
    program.validate();

    return program;
}

Program& ResourceManager::insertProgram(const std::string& name,
                                        const std::string& vertPath,
                                        const std::string& fragPath) {
    massert(m_programs.find(name) == m_programs.end(), "Cannot insert duplicate program {}", name);

    auto [it, success] = m_programs.try_emplace(name);
    auto& program      = it->second;

    VertexShader vertexShader(vertPath);
    FragmentShader fragmentShader(fragPath);
    program.attachShader(vertexShader);
    program.attachShader(fragmentShader);
    program.validate();

    return program;
}

Texture& ResourceManager::insertTexture(const std::string& name,
                                        int binding,
                                        int size,
                                        int depth,
                                        bool dualChannel) {
    massert(m_textures.find(name) == m_textures.end(), "Cannot insert duplicate texture {}", name);

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