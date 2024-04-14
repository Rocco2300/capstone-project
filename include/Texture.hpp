#pragma once

#include "Types.hpp"

#include <imgui.h>

class Texture {
private:
    uint32 m_id{};
    bool m_generated{};

    int m_width{};
    int m_height{};
    int m_depth{};

    int m_type{};
    int m_format{};
    int m_target{};
    int m_internalFormat{};

public:
    Texture();
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    operator uint32() const;
    operator ImTextureID() const;

    int target();

    void create();
    void setData(void* data, int index = 0);
    void setSize(int width, int height, int depth = 1);
    void setFormat(int internalFormat, int format, int type);

private:
    void updateImage(void* data = nullptr, int index = 0);
    void generateTexture();
};