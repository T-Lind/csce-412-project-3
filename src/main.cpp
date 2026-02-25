/**
 * @file main.cpp
 * @brief Driver for the Bizaco Load Balancer simulation.
 * @author Bizaco Load Balancer Project
 *
 * Reads config from file / command line, builds the LB,
 * and runs the sim for the specified # of clock cycles.
 */

#include "Config.h"
#include "LoadBalancer.h"
#include "Switch.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

static bool hasSwitchMode(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i){
        if (std::strcmp(argv[i], "--switch") == 0) return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    Config cfg;
    cfg.initialQueueSize = cfg.initialServers * 100;

    if (argc >= 2 && std::string(argv[1]) != "--servers" && std::string(argv[1]) != "--config" && std::string(argv[1]).find("--") != 0)
        cfg.loadFromFile(argv[1]);
    if (cfg.configPath.empty())
        cfg.loadFromFile("config.cfg");
    cfg.applyCommandLine(argc, argv);

    bool useSwitch = hasSwitchMode(argc, argv);
    if (cfg.logPath.empty())
        cfg.logPath = useSwitch
            ? "logs/switch_" + std::to_string(cfg.runTime) + "cycles.txt"
            : "logs/run_log_" + std::to_string(cfg.initialServers) + "servers_" + std::to_string(cfg.runTime) + "cycles.txt";

    size_t slash = cfg.logPath.find_last_of("/\\");
    if (slash != std::string::npos) {
        std::string logDir = cfg.logPath.substr(0, slash);
        mkdir(logDir.c_str(), 0755);
    }

    if (useSwitch) {
        Switch sw(cfg);
        for (const auto& range : cfg.blockedRanges){
            sw.getIPBlocker().addBlockedRange(range);}
        sw.setLogStream(&std::cout);
        sw.setLogFile(cfg.logPath);
        sw.runSimulation();
        std::cout << "Switch sim done. Log written to " << cfg.logPath << std::endl;



    } else {
        LoadBalancer lb(cfg);
        for (const auto& range : cfg.blockedRanges)
            lb.getIPBlocker().addBlockedRange(range);
        lb.setLogStream(&std::cout);
        lb.setLogFile(cfg.logPath);
        lb.runSimulation();
        std::cout << "Sim complete. Log written to " << cfg.logPath << std::endl;
    }
    return 0;
}
