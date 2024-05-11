#pragma once

#include "Types.hpp"

#include <glm/glm.hpp>

#include <vector>

class Program;

class FFT {
private:
    int m_width;
    int m_height;

    uint32 m_ssbo;

    Program* m_copyProgram;
    Program* m_ifftProgram;
    Program* m_butterflyProgram;
    Program* m_invertAndPermuteProgram;

    std::vector<uint32> m_reversed;
    std::vector<std::vector<glm::vec2>> m_twiddle;

public:
    FFT(int size);

    void setSize(int size);

    void dispatchIFFT(int input, int output);
    void dispatchCPUIFFT(int input, int output);

private:
    void horizontalFFT(const std::string& input, const std::string& output, int index);
};