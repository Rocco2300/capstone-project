#pragma once

#include "Program.hpp"

class DFT {
private:
    int m_size;

    Program m_IDFT;
    Program m_verticalIDFT;
    Program m_horizontalIDFT;

public:
    DFT(int size);

    void dispatchIDFT();
    void setSize(int size);
};