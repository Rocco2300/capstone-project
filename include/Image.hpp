#pragma once

#include "Types.hpp"

#include <vector>
#include <string>

struct Pixel {
    float r, g, b, a;
};

class Image {
private:
    uint16 m_width{};
    uint16 m_height{};

    float* m_data{};

public:
    Image() = default;
    Image(uint16 width, uint16 height);
    virtual ~Image();

    Image(Image&& other);
    Image& operator=(Image&& other);

    Image(const Image& other) = delete;
    Image& operator=(const Image& other) = delete;

    Pixel& at(int index);
    Pixel& at(int x, int y);

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