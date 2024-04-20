#pragma once

class Program;

class DFT {
private:
    int m_size;

    Program* m_IDFT;
    Program* m_sines;
    Program* m_gerstner;

public:
    DFT(int size);

    void setSize(int size);

    void dispatchSines();
    void dispatchGerstner();
    void dispatchIDFT(int input, int output);

    void dispatchGerstnerCPU();
};