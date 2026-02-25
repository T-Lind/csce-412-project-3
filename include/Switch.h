/**
 * @file Switch.h
 * @brief Bonus: Switch routes reqs by type streaming/processing to 2 LBs
 * @author Bizaco Load Balancer Project
 *
 * Runs two LBs concurrently (shared clock); keeps stats for each.
 */

#ifndef SWITCH_H
#define SWITCH_H

#include "Config.h"
#include "LoadBalancer.h"
#include "Request.h"
#include "IPBlocker.h"
#include <memory>
#include <ostream>
#include <fstream>
#include <random>

/**
 * @class Switch
 * @brief Routes jobs by type: 'S' (Streaming) to one LB, 'P' (Processing) to another.
 *        Runs both LBs in lockstep for the same runTime, writing combined and per-LB stats.
 */
class Switch {
public:
    explicit Switch(const Config& cfg);

    void setLogStream(std::ostream* os);
    void setLogFile(const std::string& path);

    /** Access IP blocker for routing (blocked reqs are not sent to either LB) */
    IPBlocker& getIPBlocker() { return ipBlocker_; }

    /**
     * Run sim: generate reqs, route by type, advance both LBs each cycle
     */
    void runSimulation();

private:
    Config cfg_;
    LoadBalancer lbStreaming_;
    LoadBalancer lbProcessing_;
    IPBlocker ipBlocker_;
    int nextRequestId_{1};
    size_t totalBlocked_{0};

    std::ostream* logStream_{nullptr};
    std::ofstream logFile_;

    void generateAndRouteInitialQueue(std::mt19937& rng);
    void generateAndRouteOneCycle(std::mt19937& rng, int currentTime);
};

#endif /* SWITCH_H */
