/**
 * @file LoadBalancer.cpp
 * @brief Implementation of LoadBalancer: simulation loop, scaling, and logging.
 */

#include "LoadBalancer.h"
#include <random>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {

std::string ansiGreen()  { return "\033[32m"; }
std::string ansiRed()    { return "\033[31m"; }
std::string ansiYellow() { return "\033[33m"; }
std::string ansiCyan()   { return "\033[36m"; }
std::string ansiReset()  { return "\033[0m"; }

std::string randomIp(std::mt19937& rng) {
    std::uniform_int_distribution<int> u(0, 255);
    std::ostringstream os;
    os << u(rng) << '.' << u(rng) << '.' << u(rng) << '.' << u(rng);
    return os.str();
}

} // namespace

LoadBalancer::LoadBalancer(const Config& cfg) : cfg_(cfg) {
    if (cfg_.initialQueueSize <= 0) {  cfg_.initialQueueSize = cfg_.initialServers * 100;}
    for (int i = 0; i < cfg_.initialServers; ++i) addServer();
}

void LoadBalancer::addServer() {
    int id = static_cast<int>(servers_.size()) + 1;
    servers_.push_back(std::make_unique<WebServer>(id));
}

void LoadBalancer::removeServer() {
    if (servers_.size() <= 1) return;
    for (auto it = servers_.rbegin(); it != servers_.rend(); ++it) {
        if ((*it)->active() && !(*it)->isBusy(cT_)) {
            (*it)->setActive(false);
            return;
        }
    }
}

void LoadBalancer::setLogStream(std::ostream* os) { logStream_ = os; }




void LoadBalancer::setLogFile(const std::string& path) {
    logFile_.open(path);
    if (logFile_) logEvent("INFO", "Log file opened: " + path);
}



void LoadBalancer::generateInitialQueue(std::mt19937& rng) {
    std::uniform_int_distribution<int> svc(cfg_.minServiceTime, cfg_.maxServiceTime);
    std::uniform_int_distribution<int> type(0, 1);
    for (int i = 0; i < cfg_.initialQueueSize; ++i) {
        Request r(randomIp(rng), randomIp(rng), svc(rng), type(rng) ? 'S' : 'P', 0, nextRequestId_++);
        if (ipBlocker_.isBlocked(r.ipIn)) {
            totalBlocked_++;
            continue;
        }
        rQ_.enqueue(r);
        totGenerated_++;
    }
}

void LoadBalancer::runSimulation() {
    std::random_device rd;
    unsigned int seed = cfg_.seed != 0 ? cfg_.seed : static_cast<unsigned int>(rd());  // if seed is not set, use a random seed
    std::mt19937 rng(seed);
    generateInitialQueue(rng);

    initialQueueSize_ = rQ_.size();

    if (logFile_.is_open()) {
        logFile_ << "Run: " << cfg_.initialServers << " servers, runTime: " << cfg_.runTime << "\n";
        logFile_ << "Starting queue size: " << rQ_.size() << "\n";
        logFile_ << "Task time range: [" << cfg_.minServiceTime << ", " << cfg_.maxServiceTime << "]\n";
        logFile_ << "Seed: " << seed << "\n";
        logFile_ << "ScaleCooldown: " << cfg_.scaleCooldown << "\n";
        logFile_ << "LowFactor: " << cfg_.lowFactor << " HighFactor: " << cfg_.highFactor << "\n";
        logFile_ << "IPRangesBlocked: [";
        for (size_t i = 0; i < ipBlocker_.getBlockedRanges().size(); ++i) {
            if (i) logFile_ << ", ";
            logFile_ << ipBlocker_.getBlockedRanges()[i];
        }
        logFile_ << "]\n---\n";
        logFile_.flush();
    }

    size_t startQueueSize = rQ_.size();
    pQS_ = startQueueSize;
    pQC_ = 0;

    for (cT_ = 0; cT_ < cfg_.runTime; ++cT_) {
        maybeGenerateNewRequests(rng);
        sumQueueSize_ += rQ_.size();
        if (rQ_.size() > pQS_) {
            pQS_ = rQ_.size();
            pQC_ = cT_;
        }

        for (auto& s : servers_) {
            if (!s->active()) continue;
            if (s->isBusy(cT_)) continue;
            const Request* req = s->currentRequest();
            if (req) {
                totCompleted_++;
                if (logFile_.is_open())
                    logFile_ << "[" << std::setw(7) << std::setfill('0') << cT_ << "] COMPLETE server=" << s->getId() << " reqID=" << req->id << " queue=" << rQ_.size() << "\n";
                s->markCompleted();
            }
        }
        distributeRequests();
        if (cT_ - lST_ >= cfg_.scaleCooldown)
            scaleIfNeeded();
    }
    writeSummary();
    if (logFile_.is_open()) logFile_.close();
}

void LoadBalancer::distributeRequests() {

    Request req;
    while (rQ_.try_dequeue(req)) {
        int sid = nextFreeServerId();
        if (sid < 0) {
            rQ_.enqueue(req);
            break;
        }
        WebServer* s = servers_[static_cast<size_t>(sid)].get();
        s->assignRequest(req, cT_);
        if (logFile_.is_open())
            logFile_ << "[" << std::setw(7) << std::setfill('0') << cT_ << "] ASSIGN server=" << s->getId() << " reqID=" << req.id << " svc=" << req.serviceTime << " job=" << req.jobType << "\n";
    }
}

void LoadBalancer::scaleIfNeeded() {
    int active = activeServerCount();
    size_t q = rQ_.size();
    size_t lowThreshold = static_cast<size_t>(cfg_.lowFactor * active);
    size_t highThreshold = static_cast<size_t>(cfg_.highFactor * active);

    if (q > highThreshold && static_cast<size_t>(servers_.size()) < 100) {
        addServer();
        lST_ = cT_;
        scaleUpCount_++;
        if (logFile_.is_open())
            logFile_ << "[" << std::setw(7) << std::setfill('0') << cT_ << "] SCALE_UP newServers=" << activeServerCount() << " queueSize=" << q << "\n";
        logEvent("SCALE_UP", "newServers=" + std::to_string(activeServerCount()) + " queueSize=" + std::to_string(q));
    
    
    } else if (q < lowThreshold && active > 1) {
        removeServer();
        lST_ = cT_;
        scaleDownCount_++;
        if (logFile_.is_open())
            logFile_ << "[" << std::setw(7) << std::setfill('0') << cT_ << "] SCALE_DOWN newServers=" << activeServerCount() << " queueSize=" << q << "\n";
        logEvent("SCALE_DOWN", "newServers=" + std::to_string(activeServerCount()) + " queueSize=" + std::to_string(q));
    }
}

void LoadBalancer::maybeGenerateNewRequests(std::mt19937& rng) {
    std::uniform_int_distribution<int> percent(0, 99);
    std::uniform_int_distribution<int> svc(cfg_.minServiceTime, cfg_.maxServiceTime);
    std::uniform_int_distribution<int> type(0, 1);
    if (percent(rng) >= cfg_.newRequestProbabilityPercent) return;
    Request r(randomIp(rng), randomIp(rng), svc(rng), type(rng) ? 'S' : 'P', cT_, nextRequestId_++);
    if (ipBlocker_.isBlocked(r.ipIn)) {
        totalBlocked_++;
        if (logFile_.is_open())
            logFile_ << "[" << std::setw(7) << std::setfill('0') << cT_ << "] BLOCKED ip=" << r.ipIn << " reason=blocked-range\n";
        return;
    }
    rQ_.enqueue(r);
    totGenerated_++;
}

void LoadBalancer::enqueueRequest(const Request& r) {
    rQ_.enqueue(r);
    totGenerated_++;
}

void LoadBalancer::runOneCycleAt(int currentTime) {
    cT_ = currentTime;
    sumQueueSize_ += rQ_.size();
    if (rQ_.size() > pQS_) {
        pQS_ = rQ_.size();
        pQC_ = cT_;
    }
    for (auto& s : servers_) {
        if (!s->active()) { continue;}
        if (s->isBusy(cT_)) continue;
        const Request* req = s->currentRequest();
        if (req) {
            totCompleted_++;
            s->markCompleted();
        }
    }
    distributeRequests();
    if (cT_ - lST_ >= cfg_.scaleCooldown)
        scaleIfNeeded();
}

void LoadBalancer::writeSummaryTo(std::ostream& os, const std::string& namePrefix) const {
    writeSummaryToImpl(os, namePrefix);
}

size_t LoadBalancer::getQueueSize() const { return rQ_.size(); }
size_t LoadBalancer::getTotalCompleted() const { return totCompleted_; }
size_t LoadBalancer::getTotalGenerated() const { return totGenerated_; }

void LoadBalancer::writeSummaryToImpl(std::ostream& os, const std::string& namePrefix) const {
    if (!namePrefix.empty()) os << "---\n" << namePrefix << " LOAD BALANCER\n---\n";
    else os << "SUMMARY:\n";
    os << "End queue size: " << rQ_.size() << "\n";
    os << "Total # generated: " << totGenerated_ << "\n";
    os << "Total # completed: " << totCompleted_ << "\n";
    os << "Total # blocked: " << totalBlocked_ << "\n";
    os << "Total # rejected/discarded: " << totRejected_ << "\n";
    os << "Starting queue size: " << initialQueueSize_ << "\n";
    int active = activeServerCount();
    os << "Active servers (final): " << active << "\n";
    os << "Inactive servers (scaled down): " << (static_cast<int>(servers_.size()) - active) << "\n";
    os << "Peak queue size (pqs): " << pQS_ << " at cycle " << pQC_ << "\n";
    double avgQueue = cfg_.runTime > 0 ? static_cast<double>(sumQueueSize_) / cfg_.runTime : 0;
    os << "Avg queue size (aqs): " << std::fixed << std::setprecision(1) << avgQueue << "\n";
    double utilization = (cfg_.runTime > 0 && active > 0)
        ? (100.0 * totCompleted_ / (cfg_.runTime * static_cast<double>(active))) : 0;
    os << "Avg server utilization: " << std::fixed << std::setprecision(1) << utilization << "%\n";
    os << "Scale-up events: " << scaleUpCount_ << " Scale-down events: " << scaleDownCount_ << "\n";
    os << "RunTime (rt): " << cfg_.runTime << " cycles\n";
    os << "Task / service time range: [" << cfg_.minServiceTime << ", " << cfg_.maxServiceTime << "]\n";
}

void LoadBalancer::writeSummary() {
    if (!logFile_.is_open()) return;
    writeSummaryToImpl(logFile_, "");
}

void LoadBalancer::logEvent(const std::string& kind, const std::string& msg) {
    if (!logStream_) return;
    if (useColor_) {
        if (kind == "SCALE_UP") *logStream_ << ansiGreen();
        else if (kind == "SCALE_DOWN") *logStream_ << ansiYellow();
        else if (kind == "BLOCKED") *logStream_ << ansiRed();
        else *logStream_ << ansiCyan();
    }
    *logStream_ << "[" << kind << "] " << msg << (useColor_ ? ansiReset() : "") << "\n";
}

int LoadBalancer::activeServerCount() const {
    int n = 0;
    for (const auto& s : servers_) {
        if (s->active()) {
            n++;
        }
    }
    return n;
}

int LoadBalancer::nextFreeServerId() const {
    for (size_t i = 0; i < servers_.size(); ++i) {
        if (servers_[i]->active() && !servers_[i]->isBusy(cT_))
            return static_cast<int>(i);
    }
    return -1;
}
