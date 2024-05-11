#pragma once

#include "Types.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <string>

struct PixelStruct {
    float r, g, b, a;
};

typedef glm::vec4 Pixel;

class Image {
private:
    uint16 m_width{};
    uint16 m_height{};

    std::vector<Pixel> m_data{};

public:
    Image() = default;
    Image(uint16 width, uint16 height);
    virtual ~Image() = default;

    Image(Image&& other);
    Image& operator=(Image&& other);

    Image(const Image& other) = delete;
    Image& operator=(const Image& other) = delete;

    void setSize(uint16 width, uint16 height);

    Pixel& at(int index);
    Pixel& at(int x, int y);
    std::vector<Pixel> row(int index);
    std::vector<Pixel> column(int index);

    virtual void* data();
    virtual uint16 width();
    virtual uint16 height();
};

class NoiseImage : public Image {
public:
    NoiseImage() = default;
    NoiseImage(uint16 width, uint16 height);

    void generateNoise();

private:
};