/**
 * @file LoadBalancer.h
 * @brief Load balancer: manages req queue and web servers, scales dynamically
 * @author Bizaco Load Balancer Project
 */

#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "Config.h"
#include "Request.h"
#include "RequestQueue.h"
#include "WebServer.h"
#include "IPBlocker.h"
#include <vector>
#include <memory>
#include <ostream>
#include <fstream>
#include <random>

/**
 * @class LoadBalancer
 * @brief Manages a queue of reqs & a pool of web servers; distributes work & scales servers
 */
class LoadBalancer {
public:
    explicit LoadBalancer(const Config& cfg);

    /**
     * Add 1 web server to pool
     */
    void addServer();

    /**
     * Remove 1 web server (oldest /least busy). Does not go below 1.
     */
    void removeServer();

    /**
     * Run the full sim for cfg.runTime cycles & write logs
     */
    void runSimulation();

    /**
     * Set output stream for colored console output (optional). If null, no console logging
     */
    void setLogStream(std::ostream* os);
    void setLogFile(const std::string& path);

    /**
     * Generate initial queue (initialQueueSize reqs). Called automatically at start of runSimulation()
     */
    void generateInitialQueue(std::mt19937& rng);

    /** Access IP blocker to add blocked ranges (e.g. before runSimulation) */
    IPBlocker& getIPBlocker() { return ipBlocker_; }

    /**
     * Enqueue a req (used by Switch when routing by job type)
     */
    void enqueueRequest(const Request& r);

    /**
     * Run one sim cycle at the given time (no new req generation)
     * Used when driven by Switch for concurrent multi-LB simulation.
     */
    void runOneCycleAt(int currentTime);

    /**
     * Write summary to an output stream (e.g. for Switch combined log)
     * @param os Output stream
     * @param namePrefix Optional label (e.g. "Streaming" or "Processing")
     */
    void writeSummaryTo(std::ostream& os, const std::string& namePrefix = "") const;

    /** Current queue size (for stats) */
    size_t getQueueSize() const;
    /** Total reqs completed by this LB. */
    size_t getTotalCompleted() const;
    /** Total reqs that entered this LB (enqueued). */
    size_t getTotalGenerated() const;

private:
    Config cfg_;
    RequestQueue rQ_;
    std::vector<std::unique_ptr<WebServer>> servers_;
    IPBlocker ipBlocker_;
    int cT_{0};
    int lST_{-9999};
    int nextRequestId_{1};
    size_t initialQueueSize_ = 0;

    /** Stats */
    size_t totGenerated_{0};
    size_t totCompleted_{0};
    size_t totalBlocked_{0};
    size_t totRejected_{0};
    size_t pQS_{0};
    int pQC_{0};
    size_t sumQueueSize_{0};
    int scaleUpCount_{0};
    int scaleDownCount_{0};

    std::ostream* logStream_{nullptr};
    std::ofstream logFile_;
    bool useColor_{true};

    void distributeRequests();
    void scaleIfNeeded();
    void maybeGenerateNewRequests(std::mt19937& rng);
    void writeSummary();
    void writeSummaryToImpl(std::ostream& os, const std::string& namePrefix) const;
    void logEvent(const std::string& kind, const std::string& msg);
    int activeServerCount() const;
    int nextFreeServerId() const;
};

#endif /* LOADBALANCER_H */
