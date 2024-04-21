#pragma once

#include "Types.hpp"

#include <chrono>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#define QUERY_COUNT 80

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

struct FrameNode {
    int id{};
    std::string name{};

    TimePoint start;
    double elapsedTime{};

    std::vector<std::unique_ptr<FrameNode>> children{};

    void print(int depth);
    void add(FrameNode* frame);
    void divide(int number);
};

struct BoundQuery {
    uint32 queryID;
    FrameNode* frameNode;
};

class Profiler {
private:
    static std::string m_target;

    static std::vector<uint32> m_queryPool;
    static std::stack<FrameNode*> m_frameStack;
    static std::queue<BoundQuery> m_boundQueries;
    static std::vector<std::unique_ptr<FrameNode>> m_frames;
    static std::vector<std::unique_ptr<FrameNode>> m_results;
    static std::unordered_map<std::string, FrameNode*> m_computedFrames;

    static bool m_profiling;
    static bool m_initialized;
    static bool m_resultsAvailable;

    static int m_currentFrame;

    static double m_elapsedTime;
    static double m_profileTime;

public:
    Profiler() = delete;

    static bool profiling();
    static bool resultsAvailable();

    static void initialize();
    static void beginProfiling(const std::string& name, double seconds);

    static void frameBegin();
    static void frameEnd();
    static void functionBegin(const std::string& name);
    static void functionEnd(const std::string& name);
    static void queryBegin(const std::string& name = "");
    static void queryEnd(const std::string& name = "");
};