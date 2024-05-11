#include "FFT.hpp"

#include "Globals.hpp"
#include "Profiler.hpp"
#include "ResourceManager.hpp"

#include <GL/gl3w.h>

#include <glm/gtx/integer.hpp>
#include <glm/gtc/constants.hpp>

#include <fmt/core.h>

FFT::FFT(int size) {
    m_width  = glm::log2(size);
    m_height = size;

    computeReversals();
    computeTwiddleFactors();

    glGenBuffers(1, &m_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32) * size, &m_reversed[0], GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, REVERSED_BINDING, m_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    m_butterflyProgram = &ResourceManager::getProgram("butterfly");
    m_butterflyProgram->setUniform("size", m_height);
    m_butterflyProgram->use();
    glDispatchCompute(m_width, m_height / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_ifftProgram             = &ResourceManager::getProgram("ifft");
    m_copyProgram             = &ResourceManager::getProgram("copyTexture");
    m_invertAndPermuteProgram = &ResourceManager::getProgram("invert");
    m_invertAndPermuteProgram->setUniform("size", size);
}

void FFT::setSize(int size) {
    m_width  = glm::log2(size);
    m_height = size;

    m_butterflyProgram->setUniform("size", m_height);
    m_invertAndPermuteProgram->setUniform("size", size);

    computeReversals();
    computeTwiddleFactors();

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32) * size, &m_reversed[0], GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, REVERSED_BINDING, m_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    m_butterflyProgram->use();
    glDispatchCompute(m_width, m_height / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void FFT::dispatchIFFT(int input, int output) {
    Profiler::functionBegin("ComputeOceanSurface");

    int pingpong = 0;
    Profiler::queryBegin();
    m_ifftProgram->use();
    m_ifftProgram->setUniform("buffer0", input);
    m_ifftProgram->setUniform("buffer1", BUFFER_INDEX);
    m_ifftProgram->setUniform("direction", 0);
    for (int i = 0; i < m_width; i++) {
        m_ifftProgram->setUniform("stage", i);
        m_ifftProgram->setUniform("pingpong", pingpong);
        glDispatchCompute(m_height / THREAD_NUMBER, m_height / THREAD_NUMBER, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        pingpong = !pingpong;
    }
    Profiler::queryEnd();

    Profiler::queryBegin();
    m_ifftProgram->use();
    m_ifftProgram->setUniform("direction", 1);
    for (int i = 0; i < m_width; i++) {
        m_ifftProgram->setUniform("stage", i);
        m_ifftProgram->setUniform("pingpong", pingpong);
        glDispatchCompute(m_height / THREAD_NUMBER, m_height / THREAD_NUMBER, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        pingpong = !pingpong;
    }
    Profiler::queryEnd();

    Profiler::queryBegin();
    m_invertAndPermuteProgram->use();
    m_invertAndPermuteProgram->setUniform("pingpong", pingpong);
    m_invertAndPermuteProgram->setUniform("buffer0", input);
    m_invertAndPermuteProgram->setUniform("buffer1", BUFFER_INDEX);
    glDispatchCompute(m_height / THREAD_NUMBER, m_height / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    Profiler::queryEnd();

    // final output is in buffer not in input
    // if doing multiple fft buffer will be reused
    // as such will delete previous work
    auto in = (pingpong == 1) ? BUFFER_INDEX : input;

    Profiler::queryBegin();
    m_copyProgram->use();
    m_copyProgram->setUniform("to", output);
    m_copyProgram->setUniform("from", in);
    m_copyProgram->setUniform("toDebugView", 0);
    glDispatchCompute(m_height / THREAD_NUMBER, m_height / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    Profiler::queryEnd();

    Profiler::functionEnd("ComputeOceanSurface");
}

void FFT::dispatchCPUIFFT(int input, int output) {
    int pingpong = 0;
    for (int i = 0; i < m_height; i++) {

    }
    //for (int i = 0; i < m_twiddle.size(); i++) {
    //    auto& m_twiddleRow = m_twiddle[i];
    //    for (int j = 0; j < m_twiddleRow.size(); j++) {
    //        fmt::print("{},{} ", m_twiddleRow[j].x, m_twiddleRow[j].y);
    //    }
    //    fmt::print("\n");
    //}
}

void FFT::horizontalFFT(const std::string& input, const std::string& output, int index) {
    auto& image = ResourceManager::getImage(input);

}