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

//std::vector<std::unique_ptr<FrameNode>> Profiler::m_results;
//std::unordered_map<std::string, FrameNode*> Profiler::m_computedFrames;

bool Profiler::m_profiling{};
bool Profiler::m_initialized{};
bool Profiler::m_resultsAvailable{};

double Profiler::m_elapsedTime{};
double Profiler::m_profileTime{};

bool Profiler::profiling() { return m_profiling; }

bool Profiler::resultsAvailable() { return m_resultsAvailable; }

void Profiler::initialize() {
    m_profiling        = false;
    m_initialized      = true;
    m_resultsAvailable = true;
    m_queryPool.resize(QUERY_COUNT);

    glGenQueries(QUERY_COUNT, &m_queryPool[0]);

    static uint32 dummyQueryID;
    glGenQueries(1, &dummyQueryID);
    glQueryCounter(dummyQueryID, GL_TIMESTAMP);
}

void Profiler::beginProfiling(const std::string& name, double seconds) {
    massert(m_initialized == true, "Profiler not initialized before starting profiling!\n");

    m_target           = name;
    m_profiling        = true;
    m_resultsAvailable = false;

    m_currentFrame = 0;
    m_elapsedTime  = 0.0;
    m_profileTime  = seconds * 1000.0;

    //m_frames.clear();
}

void Profiler::frameBegin() {
    auto timeStamp = std::chrono::high_resolution_clock::now();
    if (!m_initialized || !m_profiling) {
        return;
    }

    m_frameStart = timeStamp;

    //auto* frame  = new FrameNode();
    //frame->start = timeStamp;
    //frame->id    = m_currentFrame;

    //m_frames.push_back(std::unique_ptr<FrameNode>(frame));
    //m_frameStack.emplace(frame);
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

        if (!m_profiling && m_boundQueries.empty()) {
            m_resultsAvailable = true;

            m_frameTime /= m_currentFrame;
            for (auto& [name, function]: m_functions) {
                function.elapsedTime /= function.callCount;
            }

            fmt::print("{}: {:.3f} ms\n", m_target, m_frameTime);
            for (auto& [name, function]: m_functions) {
                fmt::print("\t{}: {:.3f} ms\n", name, function.elapsedTime);
            }
        }
    }

    if (!m_initialized || !m_profiling) {
        return;
    }

    std::chrono::duration<double, std::milli> time(timeStamp - m_frameStart);
    m_frameTime += time.count();

    //auto* frame = m_frameStack.top();
    //std::chrono::duration<double, std::milli> time(timeStamp - frame->start);
    //frame->elapsedTime += time.count();
    //m_elapsedTime += time.count();
    //m_frameStack.pop();

    m_currentFrame++;
    m_elapsedTime += time.count();
    //m_computedFrames.clear();
    if (m_elapsedTime >= m_profileTime) {
        m_profiling = false;
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

    //FrameNode* function;
    //auto* parent = m_frameStack.top();
    //if (m_computedFrames.count(name)) {
    //    function = m_computedFrames[name];
    //} else {
    //    function = new FrameNode();
    //    parent->children.push_back(std::unique_ptr<FrameNode>(function));
    //    m_computedFrames.emplace(name, function);
    //}

    //function->start = timeStamp;
    //function->name  = name;
    //m_frameStack.emplace(function);
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

    //auto* function = m_frameStack.top();
    //std::chrono::duration<double, std::milli> time(timeStamp - function->start);
    //function->elapsedTime += time.count();
    //m_frameStack.pop();
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

    //FrameNode* function;
    //if (!name.empty()) {
    //    auto* parent   = m_frameStack.top();
    //    function       = new FrameNode();
    //    function->name = name;
    //    parent->children.push_back(std::unique_ptr<FrameNode>(function));
    //    m_frameStack.emplace(function);
    //} else {
    //    function = m_frameStack.top();
    //}
    //m_boundQueries.emplace(queryID, function);
}

void Profiler::queryEnd(const std::string& name) {
    if (!m_initialized || !m_profiling) {
        return;
    }
    glEndQuery(GL_TIME_ELAPSED);

    //if (!name.empty()) {
    //    massert(m_frameStack.top()->name == name,
    //            "Ending profiling of different query.\nExpected: {}\nGot: {}\n",
    //            m_frameStack.top()->name, name);
    //    m_frameStack.pop();
    //}
}

//void Profiler::printResults() {
//    std::unordered_set<std::string> seen;
//    for (auto it = m_results.rbegin(); it != m_results.rend(); it++) {
//        const auto& frameNode = *it;
//        if (seen.count(frameNode->name)) {
//            continue;
//        }
//
//        frameNode->print(0);
//        seen.emplace(frameNode->name);
//    }
//}
//
//void Profiler::printResult(std::string_view name) {
//    auto it = std::find_if(m_results.rbegin(), m_results.rend(), [name](const auto& el) {
//        return el->name == name;
//    });
//
//    if (it != m_results.rend()) {
//        auto& frameNode = *it;
//        frameNode->print(0);
//    }
//}