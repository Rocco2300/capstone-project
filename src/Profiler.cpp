#include "Profiler.hpp"

#include "Assert.hpp"

#include <GL/gl3w.h>

#include <iomanip>
#include <thread>
#include <unordered_set>

void FrameNode::print(int depth) {
    for (int i = 0; i < depth; i++) { std::cout << '\t'; }

    if (name.empty()) {
        std::cout << "Frame #" << id << ": ";
    } else {
        std::cout << name << ": ";
    }
    std::cout << "CPU " << cpuTime << "ms GPU " << gpuTime << "ms\n";

    for (auto& child: children) { child->print(depth + 1); }
}

std::vector<uint32> Profiler::m_queryPool;
std::vector<FrameNode*> Profiler::m_frames;
std::queue<BoundQuery> Profiler::m_boundQueries;
std::stack<FrameNode*> Profiler::m_frameStack;

bool Profiler::m_profiling{};
bool Profiler::m_initialized{};
bool Profiler::m_resultsAvailable{};

int Profiler::m_currentFrame{};
int Profiler::m_profileFrames{};

void Profiler::initialize() {
    m_initialized = true;
    m_queryPool.resize(QUERY_COUNT);

    glGenQueries(QUERY_COUNT, &m_queryPool[0]);

    static uint32 dummyQueryID;
    glGenQueries(1, &dummyQueryID);
    glQueryCounter(dummyQueryID, GL_TIMESTAMP);
}

void Profiler::beginProfiling(int frames) {
    massert(m_initialized == true, "Profiler not initialized before starting profiling!\n");

    m_profiling        = true;
    m_resultsAvailable = false;

    m_currentFrame  = 0;
    m_profileFrames = frames;
}

void Profiler::frameBegin() {
    auto timeStamp = std::chrono::high_resolution_clock::now();
    if (!m_initialized || !m_profiling) {
        return;
    }

    auto* frame  = new FrameNode();
    frame->start = timeStamp;
    frame->id    = m_currentFrame;

    m_frames.push_back(frame);
    m_frameStack.emplace(frame);
}

void Profiler::frameEnd() {
    auto timeStamp = std::chrono::high_resolution_clock::now();
    if (!m_boundQueries.empty()) {
        int done{};
        do {
            auto [queryID, frameNode] = m_boundQueries.front();
            glGetQueryObjectiv(queryID, GL_QUERY_RESULT_AVAILABLE, &done);
            if (done) {
                uint64 timer{};
                glGetQueryObjectui64v(queryID, GL_QUERY_RESULT, &timer);

                double elapsedTime = static_cast<double>(timer) / 1'000'000.0;
                frameNode->gpuTime += elapsedTime;

                m_queryPool.push_back(queryID);
                m_boundQueries.pop();
            }
        } while (done && !m_boundQueries.empty());

        if (!m_profiling && m_boundQueries.empty()) {
            m_resultsAvailable = true;
            for (const auto& frame: m_frames) { frame->print(0); }
        }
    }

    if (!m_initialized || !m_profiling) {
        return;
    }

    auto* frame = m_frameStack.top();
    std::chrono::duration<double, std::milli> time(timeStamp - frame->start);
    frame->cpuTime += time.count();
    m_frameStack.pop();

    m_currentFrame++;
    if (m_currentFrame == m_profileFrames) {
        m_profiling = false;
    }
}

void Profiler::functionBegin(const std::string& name) {
    auto timeStamp = std::chrono::high_resolution_clock::now();
    if (!m_initialized || !m_profiling) {
        return;
    }

    auto* parent    = m_frameStack.top();
    auto* function  = new FrameNode();
    function->start = timeStamp;
    function->name  = name;

    parent->children.push_back(function);
    m_frameStack.emplace(function);
}

void Profiler::functionEnd(const std::string& name) {
    auto timeStamp = std::chrono::high_resolution_clock::now();
    if (!m_initialized || !m_profiling) {
        return;
    }
    massert(m_frameStack.top()->name == name,
            "Ending profiling of different function.\nExpected: {}\nGot: {}\n",
            m_frameStack.top()->name, name);

    auto* function = m_frameStack.top();
    std::chrono::duration<double, std::milli> time(timeStamp - function->start);
    function->cpuTime += time.count();
    m_frameStack.pop();
}

void Profiler::queryBegin(const std::string& name) {
    if (!m_initialized || !m_profiling) {
        return;
    }
    auto queryID = m_queryPool.back();
    m_queryPool.pop_back();

    glBeginQuery(GL_TIME_ELAPSED, queryID);

    FrameNode* function;
    if (!name.empty()) {
        auto* parent   = m_frameStack.top();
        function       = new FrameNode();
        function->name = name;
        parent->children.push_back(function);
        m_frameStack.emplace(function);
    } else {
        function = m_frameStack.top();
    }
    m_boundQueries.emplace(queryID, function);
}

void Profiler::queryEnd(const std::string& name) {
    if (!m_initialized || !m_profiling) {
        return;
    }
    glEndQuery(GL_TIME_ELAPSED);

    if (!name.empty()) {
        massert(m_frameStack.top()->name == name,
                "Ending profiling of different query.\nExpected: {}\nGot: {}\n",
                m_frameStack.top()->name, name);
        m_frameStack.pop();
    }
}