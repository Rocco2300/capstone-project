#include "Texture.hpp"

#include "GL/gl3w.h"

#include <iostream>

Texture::Texture() {
    m_width  = 1024;
    m_height = 1024;

    m_type           = GL_UNSIGNED_INT;
    m_format         = GL_RGBA;
    m_internalFormat = GL_RGBA;

    generateTexture();
}

Texture::Texture(int internalFormat, int width, int height, int format, int type) {
    m_width = width;
    m_height = height;

    m_type = type;
    m_format = format;
    m_internalFormat = internalFormat;

    generateTexture();
}

Texture::~Texture() {
    glDeleteTextures(1, &m_id);
}

Texture::operator uint32() const { return m_id; }

Texture::operator ImTextureID() const { return reinterpret_cast<ImTextureID>(m_id); }

void Texture::setData(void* data) {
    m_data = data;
    updateImage();
}

void Texture::setWrapping(int s, int t) {
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setFilter(int min, int mag) {
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setSize(int width, int height) {
    m_width = width;
    m_height = height;

    updateImage();
}

void Texture::setFormat(int internalFormat, int format, int type) {
    m_type = type;
    m_format = format;
    m_internalFormat = internalFormat;

    updateImage();
}

void Texture::updateImage() {
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type,
                 m_data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::generateTexture() {
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    updateImage();
}