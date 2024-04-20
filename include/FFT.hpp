#pragma once

class Program;

class FFT {
private:
    int m_width;
    int m_height;

    Program* m_copyProgram;
    Program* m_ifftProgram;
    Program* m_butterflyProgram;
    Program* m_invertAndPermuteProgram;

public:
    FFT(int size);

    void dispatchIFFT(int input, int output);
    void setSize(int size);
};