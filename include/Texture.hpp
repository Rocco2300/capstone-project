#pragma once

#include "Types.hpp"

#include <imgui.h>

class Texture {
private:
    uint32 m_id{};

    int m_width{};
    int m_height{};

    uint32 m_type{};
    uint32 m_format{};
    int m_internalFormat{};

public:
    Texture();
    Texture(int internalFormat, int width, int height, uint32 format, uint32 type);

    operator uint32() const;
    operator ImTextureID() const;

    void setWrapping(int s, int t);
    void setFilter(int min, int mag);
    void setSize(int width, int height);
    void setFormat(int internalFormat, uint32 format, uint32 type);

private:
    void updateImage();
    void generateTexture();
};