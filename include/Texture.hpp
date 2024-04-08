#pragma once

#include "Types.hpp"

#include <imgui.h>

class Texture {
private:
    uint32 m_id{};

    int m_width{};
    int m_height{};

    void* m_data{};
    int m_type{};
    int m_format{};
    int m_internalFormat{};

public:
    Texture();
    Texture(int internalFormat, int width, int height, int format, int type);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    operator uint32() const;
    operator ImTextureID() const;

    void setData(void* data);
    void setWrapping(int s, int t);
    void setFilter(int min, int mag);
    void setSize(int width, int height);
    void setFormat(int internalFormat, int format, int type);

private:
    void updateImage();
    void generateTexture();
};