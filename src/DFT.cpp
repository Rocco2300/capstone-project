#include "DFT.hpp"

#include "Shader.hpp"
#include "Globals.hpp"

#include <GL/gl3w.h>

DFT::DFT(int size) {
    m_size = size;

    ComputeShader idftHShader;
    idftHShader.load("../shaders/IDFT_horizontal.comp");
    m_horizontalIDFT.attachShader(idftHShader);
    m_horizontalIDFT.validate();
    m_horizontalIDFT.setUniform("size", size);

    ComputeShader idftVShader;
    idftVShader.load("../shaders/IDFT_vertical.comp");
    m_verticalIDFT.attachShader(idftVShader);
    m_verticalIDFT.validate();
    m_verticalIDFT.setUniform("size", size);
    m_verticalIDFT.setUniform("scale", 1);
    m_verticalIDFT.setUniform("permute", 1);
}

void DFT::dispatchIDFT() {
    m_horizontalIDFT.use();
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_verticalIDFT.use();
    glDispatchCompute(m_size / THREAD_NUMBER, m_size / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void DFT::setSize(int size) {
    m_size = size;

    m_verticalIDFT.setUniform("size", size);
    m_horizontalIDFT.setUniform("size", size);
}