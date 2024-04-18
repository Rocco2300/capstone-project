#include "Image.hpp"

#include <random>
#include <fstream>

static float normalRandom() {
    static std::random_device rnd;
    static std::mt19937 gen(rnd());
    static std::normal_distribution<float> dist(-1.0f, 1.0f);

    return dist(gen);
}

Image::Image(uint16 width, uint16 height) {
    m_width  = width;
    m_height = height;

    m_data = new float[4 * width * height];
    for (int i = 0; i < width * height; i++) {
        auto index  = i * 4;
        auto& pixel = this->at(index);
        pixel.r     = 0.f;
        pixel.g     = 0.f;
        pixel.b     = 0.f;
        pixel.a     = 1.f;
    }
}

Image::Image(Image&& other) {
    m_width  = other.m_width;
    m_height = other.m_height;

    m_data       = other.m_data;
    other.m_data = nullptr;
}

Image& Image::operator=(Image&& other) {
    m_width  = other.m_width;
    m_height = other.m_height;

    m_data       = other.m_data;
    other.m_data = nullptr;
    return *this;
}

Image::~Image() {
    delete[] m_data;
    m_data = nullptr;
}

Pixel& Image::at(int index) { return *reinterpret_cast<Pixel*>(m_data + index); }

Pixel& Image::at(int x, int y) {
    int index = (y * m_width + x) * 4;
    return *reinterpret_cast<Pixel*>(m_data + index);
}

void* Image::data() { return &m_data[0]; }

uint16 Image::width() { return m_width; }

uint16 Image::height() { return m_height; }

NoiseImage::NoiseImage(uint16 width, uint16 height)
    : Image(width, height) {
    generateNoise();
}

void NoiseImage::generateNoise() {
    auto width  = this->width();
    auto height = this->height();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Pixel& pixel = this->at(x, y);
            pixel.r      = (normalRandom() + 1.f) / 2.f;
            pixel.g      = (normalRandom() + 1.f) / 2.f;
        }
    }
}