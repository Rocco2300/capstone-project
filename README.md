# Ocean Wave Simulation Performance Comparison

## Overview
This project aims to compare the performance of different methods for simulating ocean waves from oceanographic spectra. Specifically, it focuses on comparing the Discrete Fourier Transform (DFT) and Fast Fourier Transform (FFT) methods. It is using the Phillips spectrum, even though it is not accurate to real world conditions it fits within the purposes of the work done here. 

The project aims to compare the performance of the GPGPU versions, implemented using OpenGL compute shaders and CPU versions aswell. Additionally, performance metrics such as simulation time and computational efficiency will be analyzed with the help Python scripts.

Keep in mind this project is still in progress, it is missing most of the core functionality needed, like the algorithms and benchmarking...and profiling...and obviously the comparison. 

## Requirements
- GCC 13.2.0 (or another compiler, cannot ensure proper functionality)
- CMake 3.25.0
- Support for OpenGL 4.6

## Directory Structure
- **src/**: Contains C++ source files.
- **include/**: Contains C++ include files.
- **shaders/**: Contains OpenGL compute and visualization shaders.
- **external/**: Contains external dependencies (glm, GLFW, GL3W, fmt, ImGui).
- **build/**: Directory for build output.

## Build
1. Clone the repository including the dependencies
```shell
git clone --recurse-submodules https://github.com/Rocco2300/capstone-project
```
2. Go to the cloned repo directory and create a build folder
```shell
cd capstone-project
mkdir build
```
3. Use cmake to build project
```shell
cd build
cmake ..
cmake --build .
```
4. Now you can run it 
```shell
./capstone-project.exe
```

## References
- Horvath, C. J. (2015). Empirical directional wave spectra for computer graphics. Proceedings of the 2015 Symposium on Digital Production - DigiPro  ’15. doi:10.1145/2791261.2791267
- Simulating Ocean Water - Jerry Tessendorf (https://people.computing.clemson.edu/~jtessen/reports/papers_files/coursenotes2004.pdf)
- Ivan Pensionerov's implementation in Unity (https://github.com/gasgiant/FFT-Ocean) 
- Discrete Fourier Transform wikipedia page (https://en.wikipedia.org/wiki/Discrete_Fourier_transform)