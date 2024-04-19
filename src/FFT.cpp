#include "FFT.hpp"

#include "Globals.hpp"
#include "Shader.hpp"
#include "Types.hpp"

#include <GL/gl3w.h>

#include <glm/gtx/integer.hpp>

#include <memory>

std::unique_ptr<uint32[]> computeReversals(int size) {
    int width  = glm::log2(size);
    int height = size;

    auto res = std::make_unique<uint32[]>(height);
    for (int i = 0; i < height; i++) {
        int index        = i;
        unsigned int num = 0;
        for (int j = 0; j < width; j++) {
            num = (num << 1) + (index & 1);
            index >>= 1;
        }
        res[i] = num;
    }
    return res;
}

FFT::FFT(int size) {
    m_width  = glm::log2(size);
    m_height = size;

    auto reversal = computeReversals(size);
    unsigned int ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32) * size, reversal.get(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, REVERSED_BINDING, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    ComputeShader butterflyShader("../shaders/ButterflyTexture.comp");
    m_butterflyProgram.attachShader(butterflyShader);
    m_butterflyProgram.validate();
    m_butterflyProgram.setUniform("size", m_height);
    m_butterflyProgram.use();
    glDispatchCompute(m_width, m_height / 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    ComputeShader ifftShader("../shaders/IFFT.comp");
    m_ifftProgram.attachShader(ifftShader);
    m_ifftProgram.validate();

    ComputeShader copyShader("../shaders/CopyTexture.comp");
    m_copyProgram.attachShader(copyShader);
    m_copyProgram.validate();

    ComputeShader invertAndPermuteShader("../shaders/InvertAndPermute.comp");
    m_invertAndPermuteProgram.attachShader(invertAndPermuteShader);
    m_invertAndPermuteProgram.validate();
    m_invertAndPermuteProgram.setUniform("size", size);
}

void FFT::dispatchIFFT(int input, int output) {
    int pingpong = 0;
    m_ifftProgram.setUniform("buffer0", input);
    m_ifftProgram.setUniform("buffer1", BUFFER_INDEX);
    m_ifftProgram.setUniform("direction", 0);
    for (int i = 0; i < m_width; i++) {
        m_ifftProgram.setUniform("stage", i);
        m_ifftProgram.setUniform("pingpong", pingpong);
        m_ifftProgram.use();
        glDispatchCompute(m_height / THREAD_NUMBER, m_height / THREAD_NUMBER, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        pingpong = !pingpong;
    }

    m_ifftProgram.setUniform("direction", 1);
    for (int i = 0; i < m_width; i++) {
        m_ifftProgram.setUniform("stage", i);
        m_ifftProgram.setUniform("pingpong", pingpong);
        m_ifftProgram.use();
        glDispatchCompute(m_height / THREAD_NUMBER, m_height / THREAD_NUMBER, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        pingpong = !pingpong;
    }

    m_invertAndPermuteProgram.use();
    m_invertAndPermuteProgram.setUniform("pingpong", pingpong);
    m_invertAndPermuteProgram.setUniform("buffer0", input);
    m_invertAndPermuteProgram.setUniform("buffer1", BUFFER_INDEX);
    glDispatchCompute(m_height / THREAD_NUMBER, m_height / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // final output is in buffer not in input
    // if doing multiple fft buffer will be reused
    // as such will delete previous work
    auto in = (pingpong == 1) ? BUFFER_INDEX : input;
    m_copyProgram.use();
    m_copyProgram.setUniform("to", output);
    m_copyProgram.setUniform("from", in);
    glDispatchCompute(m_height / THREAD_NUMBER, m_height / THREAD_NUMBER, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void FFT::setSize(int size) {
    m_width  = glm::log2(size);
    m_height = size;

    m_butterflyProgram.setUniform("size", m_height);
    m_invertAndPermuteProgram.setUniform("size", size);
}