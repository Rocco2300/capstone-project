#include "Profiler.hpp"

#include "Assert.hpp"

#include <GL/gl3w.h>

#include <thread>

std::string Profiler::m_target;

int Profiler::m_currentFrame{};
double Profiler::m_frameTime{};
TimePoint Profiler::m_frameStart;

std::vector<uint32> Profiler::m_queryPool;
std::queue<BoundQuery> Profiler::m_boundQueries;
std::stack<std::string> Profiler::m_functionStack;
std::unordered_map<std::string, ProfiledFunction> Profiler::m_functions;
std::unordered_map<std::string, ProfilerResult> Profiler::m_results;

bool Profiler::m_profiling{};
bool Profiler::m_initialized{};
bool Profiler::m_doneProfiling{};
bool Profiler::m_resultsAvailable{};
bool Profiler::m_shouldStartProfiling{};

double Profiler::m_elapsedTime{};
double Profiler::m_profileTime{};

bool Profiler::profiling() { return m_profiling; }

bool Profiler::resultsAvailable() { return m_resultsAvailable; }

void Profiler::initialize() {
    m_profiling            = false;
    m_initialized          = true;
    m_doneProfiling        = false;
    m_resultsAvailable     = true;
    m_shouldStartProfiling = false;
    m_queryPool.resize(QUERY_COUNT);

    glGenQueries(QUERY_COUNT, &m_queryPool[0]);

    static uint32 dummyQueryID;
    glGenQueries(1, &dummyQueryID);
    glQueryCounter(dummyQueryID, GL_TIMESTAMP);
}

void Profiler::beginProfiling(const std::string& name, double seconds) {
    massert(m_initialized == true, "Profiler not initialized before starting profiling!\n");
    if (m_profiling || m_shouldStartProfiling) {
        return;
    }

    m_target               = name;
    m_profiling            = false;
    m_doneProfiling        = false;
    m_resultsAvailable     = false;
    m_shouldStartProfiling = true;

    m_frameTime    = 0.0;
    m_currentFrame = 0;
    m_elapsedTime  = 0.0;
    m_profileTime  = seconds * 1000.0;

    m_functions.clear();
}

void Profiler::frameBegin() {
    auto timeStamp = std::chrono::high_resolution_clock::now();
    if (m_shouldStartProfiling) {
        m_profiling            = true;
        m_shouldStartProfiling = false;
    }

    if (!m_initialized || !m_profiling) {
        return;
    }

    m_frameStart = timeStamp;
}

void Profiler::frameEnd() {
    auto timeStamp = std::chrono::high_resolution_clock::now();
    if (!m_boundQueries.empty()) {
        int resultsAvailable{};
        do {
            auto [queryID, frameNode] = m_boundQueries.front();
            glGetQueryObjectiv(queryID, GL_QUERY_RESULT_AVAILABLE, &resultsAvailable);
            if (resultsAvailable) {
                uint64 timer{};
                glGetQueryObjectui64v(queryID, GL_QUERY_RESULT, &timer);

                double elapsedTime = static_cast<double>(timer) / 1'000'000.0;
                frameNode->elapsedTime += elapsedTime;

                m_queryPool.push_back(queryID);
                m_boundQueries.pop();
            }
        } while (resultsAvailable && !m_boundQueries.empty());

        if (m_boundQueries.empty() && m_doneProfiling) {
            saveResults();
        }
    }

    if (!m_initialized || !m_profiling) {
        return;
    }

    std::chrono::duration<double, std::milli> time(timeStamp - m_frameStart);
    m_frameTime += time.count();
    m_elapsedTime += time.count();

    m_currentFrame++;
    auto deltaTime = m_frameTime / m_currentFrame;
    if (m_elapsedTime >= m_profileTime || deltaTime >= 33.0) {
        m_profiling     = false;
        m_doneProfiling = true;

        if (m_boundQueries.empty()) {
            saveResults();
        }
    }
}

void Profiler::functionBegin(const std::string& name) {
    auto timeStamp = std::chrono::high_resolution_clock::now();
    if (!m_initialized || !m_profiling) {
        return;
    }

    auto& function = m_functions[name];
    function.start = timeStamp;

    m_functionStack.emplace(name);
}

void Profiler::functionEnd(const std::string& name) {
    auto timeStamp = std::chrono::high_resolution_clock::now();
    if (!m_initialized || !m_profiling) {
        return;
    }

    massert(m_functionStack.top() == name,
            "Ending profiling of different function.\nExpected: {}\nGot: {}\n",
            m_functionStack.top(), name);

    auto& function = m_functions[name];
    std::chrono::duration<double, std::milli> time(timeStamp - function.start);
    function.elapsedTime += time.count();
    function.callCount++;

    m_functionStack.pop();
}

void Profiler::queryBegin(const std::string& name) {
    if (!m_initialized || !m_profiling) {
        return;
    }
    auto queryID = m_queryPool.back();
    m_queryPool.pop_back();

    glBeginQuery(GL_TIME_ELAPSED, queryID);

    auto functionName = (m_functionStack.empty()) ? name : m_functionStack.top();
    auto& function    = m_functions[functionName];

    if (!name.empty()) {
        function.callCount++;
    }

    m_boundQueries.emplace(queryID, &function);
}

void Profiler::queryEnd() {
    if (!m_initialized || !m_profiling) {
        return;
    }
    glEndQuery(GL_TIME_ELAPSED);
}

void Profiler::printResults() {
    for (const auto& [profileName, results]: m_results) {
        fmt::print("{}: {:.3f} ms\n", profileName, results.frametime);
        for (const auto& [functionName, function]: results.functions) {
            fmt::print("\t{}: {:.3f} ms\n", functionName, function.elapsedTime);
        }
    }
}

void Profiler::printResult(const std::string& name) {
    auto it = m_results.find(name);
    if (it == m_results.end()) {
        return;
    }

    auto& [profileName, results] = *it;
    fmt::print("{}: {:.3f} ms\n", profileName, results.frametime);
    for (const auto& [functionName, function]: results.functions) {
        fmt::print("\t{}: {:.3f} ms\n", functionName, function.elapsedTime);
    }
}

void Profiler::saveResults() {
    m_resultsAvailable = true;

    m_frameTime /= m_currentFrame;
    for (auto& [name, function]: m_functions) { function.elapsedTime /= function.callCount; }

    auto [it, success] = m_results.try_emplace(m_target, m_frameTime, m_functions);
    if (!success) {
        it->second = {m_frameTime, m_functions};
    }

    printResults();
}