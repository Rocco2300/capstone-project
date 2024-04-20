#include "Texture.hpp"

#include "Assert.hpp"

#include "GL/gl3w.h"

#include <iostream>

Texture::Texture() {
    m_width  = 1024;
    m_height = 1024;

    m_type           = GL_UNSIGNED_INT;
    m_format         = GL_RGBA;
    m_target         = GL_TEXTURE_2D;
    m_internalFormat = GL_RGBA;
}

Texture::~Texture() { glDeleteTextures(1, &m_id); }

Texture::operator uint32() const { return m_id; }

Texture::operator ImTextureID() const { return reinterpret_cast<ImTextureID>(m_id); }

int Texture::target() { return m_target; }

glm::vec3 Texture::size() { return {m_width, m_height, m_depth}; }

void Texture::create() {
    m_generated = true;

    generateTexture();
    updateImage();
}

void Texture::setData(void* data, int index) {
    massert(m_generated == true, "Cannot set data before creating texture!\n");
    updateImage(data, index);
}

void Texture::setSize(int width, int height, int depth) {
    m_width  = width;
    m_height = height;
    m_depth  = (m_generated) ? m_depth : depth; // don't change layer number at runtime

    // don't change target at runtime
    if (m_depth > 1 && !m_generated) {
        m_target = GL_TEXTURE_2D_ARRAY;
    }

    if (m_generated) {
        updateImage();
    }
}

void Texture::setFormat(int internalFormat, int format, int type) {
    m_type           = type;
    m_format         = format;
    m_internalFormat = internalFormat;
}

void Texture::updateImage(void* data, int index) {
    glBindTexture(m_target, m_id);

    if (m_target == GL_TEXTURE_2D) {
        glTexImage2D(m_target, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, data);
    } else {
        glTexImage3D(m_target, 0, m_internalFormat, m_width, m_height, m_depth, 0, m_format, m_type,
                     nullptr);
    }

    if (m_target == GL_TEXTURE_2D_ARRAY && data != nullptr) {
        glTextureSubImage3D(m_id, 0, 0, 0, index, m_width, m_height, 1, m_format, m_type, data);
    }
    glBindTexture(m_target, 0);
}

void Texture::generateTexture() {
    glGenTextures(1, &m_id);
    glBindTexture(m_target, m_id);

    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}