#pragma once

#include "Types.hpp"

#include <chrono>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>

// round( 80 / frame * 2, pow (2, n) )
#define QUERY_COUNT 80

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

struct FrameNode {
    int id{};
    std::string name{};

    TimePoint start;
    double cpuTime{};
    double gpuTime{};

    std::vector<FrameNode*> children{};

    void print(int depth);
};

struct BoundQuery {
    uint32 queryID;
    FrameNode* frameNode;
};

class Profiler {
private:
    static std::vector<FrameNode*> m_frames;

    static std::vector<uint32> m_queryPool;
    //static std::vector<ProfilerFrame> m_frames;
    static std::queue<BoundQuery> m_boundQueries;
    static std::stack<FrameNode*> m_frameStack;

    static bool m_profiling;
    static bool m_initialized;
    static bool m_resultsAvailable;

    static int m_currentFrame;
    static int m_profileFrames;

public:
    Profiler() = delete;

    static void initialize();
    static void beginProfiling(int frames);

    static void frameBegin();
    static void frameEnd();
    static void functionBegin(const std::string& name);
    static void functionEnd(const std::string& name);
    static void queryBegin(const std::string& name = "");
    static void queryEnd(const std::string& name = "");
};