#include "DFT.hpp"

#include "Shader.hpp"
#include "Globals.hpp"
#include "ResourceManager.hpp"

#include <GL/gl3w.h>

#include <glm/gtc/constants.hpp>

DFT::DFT(int size) {
    m_size = size;

    ComputeShader idftShader;
    idftShader.load("../shaders/IDFT.comp");
    m_IDFT.attachShader(idftShader);
    m_IDFT.validate();
    m_IDFT.setUniform("size", size);

    ComputeShader sinesShader;
    sinesShader.load("../shaders/Sines.comp");
    m_sines.attachShader(sinesShader);
    m_sines.validate();
    m_sines.setUniform("size", size);

    ComputeShader gerstnerShader;
    gerstnerShader.load("../shaders/Gerstner.comp");
    m_gerstner.attachShader(gerstnerShader);
    m_gerstner.validate();
    m_gerstner.setUniform("size", size);
}

void DFT::setSize(int size) {
    m_size = size;

    m_IDFT.setUniform("size", size);
    m_sines.setUniform("size", size);
    m_gerstner.setUniform("size", size);
}

void DFT::dispatchSines() {
    m_sines.use();

    m_sines.setUniform("direction", 0);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_sines.setUniform("direction", 1);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void DFT::dispatchGerstner() {
    m_gerstner.use();

    m_gerstner.setUniform("direction", 0);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_gerstner.setUniform("direction", 1);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void DFT::dispatchIDFT(int input, int output) {
    m_IDFT.use();
    m_IDFT.setUniform("buffer0", input);
    m_IDFT.setUniform("buffer1", output);

    m_IDFT.setUniform("direction", 0);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_IDFT.setUniform("direction", 1);
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

static glm::vec2 complexMul(glm::vec2 a, glm::vec2 b) {
    return glm::vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

void DFT::dispatchGerstnerCPU() {
    Image buffer0(m_size, m_size);
    Image buffer1(m_size, m_size);

    auto& dyImage      = ResourceManager::getImage("dy");
    auto& dx_dzImage   = ResourceManager::getImage("dx_dz");
    auto& dyx_dyzImage = ResourceManager::getImage("dyx_dyz");
    for (int y = 0; y < m_size; y++) {
        for (int x = 0; x < m_size; x++) {
            glm::vec2 h(0.0f);
            glm::vec2 d(0.0f);
            glm::vec2 n(0.0f);
            for (int k = 0; k < m_size; k++) {
                float phase = 2.0f * glm::pi<float>() * k * x / m_size;
                glm::vec2 exponent(glm::cos(phase), glm::sin(phase));

                auto& dyPix      = dyImage.at(k, y);
                auto& dx_dzPix   = dx_dzImage.at(k, y);
                auto& dyx_dyzPix = dyx_dyzImage.at(k, y);
                glm::vec2 dy(dyPix.r, dyPix.g);
                glm::vec2 dx_dz(dx_dzPix.r, dx_dzPix.g);
                glm::vec2 dyx_dyz(dyx_dyzPix.r, dyx_dyzPix.g);

                h += complexMul(dy, exponent);
                d += complexMul(dx_dz, exponent);
                n += complexMul(dyx_dyz, exponent);
            }

            buffer0.at(x, y) = {h.x, h.y, d.x, d.y};
            buffer1.at(x, y) = {n.x, n.y, 0.f, 1.f};
        }
    }
    for (int y = 0; y < m_size; y++) {
        for (int x = 0; x < m_size; x++) {
            glm::vec2 h(0.0f);
            glm::vec2 d(0.0f);
            glm::vec2 n(0.0f);
            for (int k = 0; k < m_size; k++) {
                float phase = 2.0f * glm::pi<float>() * k * y / m_size;
                glm::vec2 exponent(glm::cos(phase), glm::sin(phase));

                auto& dyx_dyzPix  = buffer1.at(x, k);
                auto& dy_dx_dzPix = buffer0.at(x, k);

                glm::vec2 dy(dy_dx_dzPix.r, dy_dx_dzPix.g);
                glm::vec2 dx_dz(dy_dx_dzPix.b, dy_dx_dzPix.a);
                glm::vec2 dyx_dyz(dyx_dyzPix.r, dyx_dyzPix.g);

                h += complexMul(dy, exponent);
                d += complexMul(dx_dz, exponent);
                n += complexMul(dyx_dyz, exponent);
            }

            float perms[] = {1.0, -1.0};
            int index     = (x + y) % 2;
            float perm    = perms[index];

            h /= static_cast<float>(m_size * m_size);
            d /= static_cast<float>(m_size * m_size);
            n /= static_cast<float>(m_size * m_size);
            h *= perm;
            d *= perm;
            n *= perm;

            glm::vec3 normal      = glm::normalize(glm::vec3(-n.x, 1.f, -n.y));
            dyImage.at(x, y)      = {h.x, h.y, 0.f, 1.f};
            dx_dzImage.at(x, y)   = {d.x, d.y, 0.f, 1.f};
            dyx_dyzImage.at(x, y) = {n.x, n.y, 0.f, 1.f};
        }
    }

    ResourceManager::getTexture("buffers").setData(dyImage.data(), HEIGHT_INDEX);
    ResourceManager::getTexture("buffers").setData(dx_dzImage.data(), DISPLACEMENT_INDEX);
    ResourceManager::getTexture("buffers").setData(dyx_dyzImage.data(), NORMAL_INDEX);
}