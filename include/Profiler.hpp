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

struct ProfiledFunction {
    TimePoint start;

    int callCount{};
    double elapsedTime{};
};

struct ProfilerResult {
    double frametime;
    std::unordered_map<std::string, ProfiledFunction> functions;
};

struct BoundQuery {
    uint32 queryID;
    ProfiledFunction* function;
};

class Profiler {
private:
    static std::string m_target;

    static int m_currentFrame;
    static double m_frameTime;
    static TimePoint m_frameStart;

    static std::vector<uint32> m_queryPool;
    static std::queue<BoundQuery> m_boundQueries;
    static std::stack<std::string> m_functionStack;
    static std::unordered_map<std::string, ProfiledFunction> m_functions;
    static std::unordered_map<std::string, ProfilerResult> m_results;

    static bool m_profiling;
    static bool m_initialized;
    static bool m_resultsAvailable;
    static bool m_shouldStartProfiling;

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
    static void queryEnd();

#ifndef NDEGBUG
    static void printResults();
    static void printResult(const std::string& name);
#endif
};