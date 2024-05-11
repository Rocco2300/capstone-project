#include "Image.hpp"

#include <cstring>
#include <fstream>
#include <random>

static float normalRandom() {
    static std::random_device rnd;
    static std::mt19937 gen(rnd());
    static std::normal_distribution<float> dist(-1.0f, 1.0f);

    return dist(gen);
}

Image::Image(uint16 width, uint16 height) {
    m_width  = width;
    m_height = height;

    m_data.resize(width * height);
    for (int i = 0; i < width * height; i++) {
        auto& pixel = m_data.at(i);
        pixel.r     = 0.f;
        pixel.g     = 0.f;
        pixel.b     = 0.f;
        pixel.a     = 1.f;
    }
}

Image::Image(Image&& other) {
    m_width  = other.m_width;
    m_height = other.m_height;

    m_data = std::move(other.m_data);
}

Image& Image::operator=(Image&& other) {
    m_width  = other.m_width;
    m_height = other.m_height;

    m_data = std::move(other.m_data);
    return *this;
}

void Image::setSize(uint16 width, uint16 height) {
    m_width  = width;
    m_height = height;

    m_data.resize(4 * width * height);
}

Pixel& Image::at(int index) { return m_data.at(index); }

Pixel& Image::at(int x, int y) {
    int index = y * m_width + x;
    return m_data.at(index);
}

std::vector<Pixel> Image::row(int index) {
    int idx = index * m_width;
    return {&m_data[idx], &m_data[idx] + m_width};
}

std::vector<Pixel> Image::column(int index) {
    std::vector<Pixel> column;
    for (int i = 0; i < m_height; i++) {
        column.push_back(this->at(index, i));
    }
    return column;
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