/**
 * @file Switch.cpp
 * @brief Implementation of swch: route by job type, run two LBs concurrently.
 */

#include "Switch.h"
#include <iomanip>
#include <sstream>

namespace {

std::string randomIp(std::mt19937& rng) {
    std::uniform_int_distribution<int> u(0, 255);
    std::ostringstream os;
    os << u(rng) << '.' << u(rng) << '.' << u(rng) << '.' << u(rng);
    return os.str();
}

} // namespace

Switch::Switch(const Config& cfg)
    : cfg_(cfg), lbStreaming_(cfg), lbProcessing_(cfg) {}

void Switch::setLogStream(std::ostream* os) {
    logStream_ = os;
    lbStreaming_.setLogStream(nullptr);
    lbProcessing_.setLogStream(nullptr);
}

void Switch::setLogFile(const std::string& path) {
    logFile_.open(path);
}

void Switch::generateAndRouteInitialQueue(std::mt19937& rng) {
    std::uniform_int_distribution<int> svc(cfg_.minServiceTime, cfg_.maxServiceTime);
    std::uniform_int_distribution<int> type(0, 1);
    for (int i = 0; i < cfg_.initialQueueSize; ++i) {
        char jobType = type(rng) ? 'S' : 'P';
        Request r(randomIp(rng), randomIp(rng), svc(rng), jobType, 0, nextRequestId_++);
        if (ipBlocker_.isBlocked(r.ipIn)) {
            totalBlocked_++;
            continue;
        }
        if (jobType == 'S')
            lbStreaming_.enqueueRequest(r);
        else
            lbProcessing_.enqueueRequest(r);
    }
}

void Switch::generateAndRouteOneCycle(std::mt19937& rng, int currentTime) {
    std::uniform_int_distribution<int> percent(0, 99);
    std::uniform_int_distribution<int> svc(cfg_.minServiceTime, cfg_.maxServiceTime);
    std::uniform_int_distribution<int> type(0, 1);
    if (percent(rng) >= cfg_.newRequestProbabilityPercent) return;
    char jobType = type(rng) ? 'S' : 'P';
    Request r(randomIp(rng), randomIp(rng), svc(rng), jobType, currentTime, nextRequestId_++);
    if (ipBlocker_.isBlocked(r.ipIn)) {
        totalBlocked_++;
        return;
    }
    if (jobType == 'S')
        lbStreaming_.enqueueRequest(r);
    else
        lbProcessing_.enqueueRequest(r);
}

void Switch::runSimulation() {
    std::random_device rd;
    unsigned int seed = cfg_.seed != 0 ? cfg_.seed : static_cast<unsigned int>(rd());
    std::mt19937 rng(seed);

    generateAndRouteInitialQueue(rng);

    if (logFile_.is_open()) {
        logFile_ << "Switch mode: Streaming + Processing load balancers\n";
        logFile_ << "RunTime: " << cfg_.runTime << " cycles\n";
        logFile_ << "Initial queue: " << cfg_.initialQueueSize << " (routed by job type S/P)\n";
        logFile_ << "Streaming LB starting queue: " << lbStreaming_.getQueueSize() << "\n";
        logFile_ << "Processing LB starting queue: " << lbProcessing_.getQueueSize() << "\n";
        logFile_ << "Task time range: [" << cfg_.minServiceTime << ", " << cfg_.maxServiceTime << "]\n";
        logFile_ << "Seed: " << seed << "\n";
        logFile_ << "Total blocked (at switch): " << totalBlocked_ << "\n";
        logFile_ << "---\n";
        logFile_.flush();
    }

    for (int t = 0; t < cfg_.runTime; ++t) {
        generateAndRouteOneCycle(rng, t);
        lbStreaming_.runOneCycleAt(t);
        lbProcessing_.runOneCycleAt(t);
    }

    if (logFile_.is_open()) {
        logFile_ << "---\nCOMBINED SUMMARY\n---\n";
        logFile_ << "Total blocked at switch: " << totalBlocked_ << "\n";
        logFile_ << "Streaming LB completed: " << lbStreaming_.getTotalCompleted()
                 << " queue: " << lbStreaming_.getQueueSize() << "\n";
        logFile_ << "Processing LB completed: " << lbProcessing_.getTotalCompleted()
                 << " queue: " << lbProcessing_.getQueueSize() << "\n";
        logFile_ << "\n";
        lbStreaming_.writeSummaryTo(logFile_, "Streaming");
        logFile_ << "\n";
        lbProcessing_.writeSummaryTo(logFile_, "Processing");
        logFile_.close();
    }

    if (logStream_) {
        *logStream_ << "Switch simulation complete. Streaming completed: "
                    << lbStreaming_.getTotalCompleted() << " Processing completed: "
                    << lbProcessing_.getTotalCompleted() << "\n";
    }
}
