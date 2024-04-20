#pragma once

#include "Image.hpp"
#include "Program.hpp"
#include "Texture.hpp"

#include <string>
#include <unordered_map>

class Image;
class Texture;

class ResourceManager {
private:
    static std::unordered_map<std::string, Image> m_images;
    static std::unordered_map<std::string, Program> m_programs;
    static std::unordered_map<std::string, Texture> m_textures;

public:
    ResourceManager() = delete;


    static void resize(int size, int depth = 1);
    static Texture& resize(const std::string& name, int size, int depth = 1);
    static Texture& getTexture(const std::string& name);
    static Texture& getDebugTexture(int index);
    static Texture& insertTexture(const std::string& name,
                                  int binding,
                                  int size,
                                  int depth        = 1,
                                  bool dualChannel = false);

    static Image& getImage(const std::string& name);
    static Image& insertImage(const std::string& name, int size);

    static Program& getProgram(const std::string& name);
    static Program& insertProgram(const std::string& name, const std::string& path);
    static Program& insertProgram(const std::string& name,
                                  const std::string& vertPath,
                                  const std::string& fragPath);
};
