/**
 * @file Config.cpp
 * @brief Load & parse configuration from file and command line.
 */

#include "Config.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>

namespace {

std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) { return "";}
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end == std::string::npos ? std::string::npos : end - start + 1);
}
int parseInt(const std::string& s, int defaultVal) {
    try {
        return std::stoi(s);
    } catch (...) {
        return defaultVal;
    }
}
unsigned int parseUInt(const std::string& s, unsigned int defaultVal) {
    try {
        return static_cast<unsigned int>(std::stoul(s));
    } catch (...) {
        return defaultVal;
    }
}

} // namespace

bool Config::loadFromFile(const std::string& path) {
    configPath = path;
    std::ifstream f(path);
    if (!f) return false;
    std::string line;


    while (std::getline(f, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = trim(line.substr(0, eq));
        std::string val = trim(line.substr(eq + 1));


        // vals of confg variables
        if (key == "initialServers") initialServers = parseInt(val, initialServers);
        else if (key == "runTime") runTime = parseInt(val, runTime);
        else if (key == "scaleCooldown") scaleCooldown = parseInt(val, scaleCooldown);
        else if (key == "initialQueueSize") initialQueueSize = parseInt(val, initialQueueSize);
        else if (key == "lowFactor") lowFactor = parseInt(val, lowFactor);
        else if (key == "highFactor") highFactor = parseInt(val, highFactor);
        else if (key == "minServiceTime") minServiceTime = parseInt(val, minServiceTime);
        else if (key == "maxServiceTime") maxServiceTime = parseInt(val, maxServiceTime);
        else if (key == "newRequestProbabilityPercent") newRequestProbabilityPercent = parseInt(val, newRequestProbabilityPercent);
        else if (key == "seed") seed = parseUInt(val, seed);
        else if (key == "logPath") logPath = val;
        else if (key == "blockedRange" || key == "blockedRanges") {
            size_t start = 0;
            while (start < val.size()) {
                size_t comma = val.find(',', start);
                std::string one = trim(comma == std::string::npos ? val.substr(start) : val.substr(start, comma - start));
                if (!one.empty()) blockedRanges.push_back(one);
                if (comma == std::string::npos) {break;}
                start = comma + 1;
            }
        }
    }
    return true;
}


void Config::applyCommandLine(int argc, char* argv[]) {

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--servers") == 0 && i + 1 < argc) {
            initialServers = parseInt(argv[++i], initialServers);
        } else if (std::strcmp(argv[i], "--runtime") == 0 && i + 1 < argc) {
            runTime = parseInt(argv[++i], runTime);
        } else if (std::strcmp(argv[i], "--cooldown") == 0 && i + 1 < argc) {
            scaleCooldown = parseInt(argv[++i], scaleCooldown);
        } else if (std::strcmp(argv[i], "--initial-queue") == 0 && i + 1 < argc) {
            initialQueueSize = parseInt(argv[++i], initialQueueSize);
        } else if (std::strcmp(argv[i], "--low") == 0 && i + 1 < argc) {
            lowFactor = parseInt(argv[++i], lowFactor);
        } else if (std::strcmp(argv[i], "--high") == 0 && i + 1 < argc) {
            highFactor = parseInt(argv[++i], highFactor);
        } else if (std::strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = parseUInt(argv[++i], seed);
        } else if (std::strcmp(argv[i], "--config") == 0 && i + 1 < argc) {
            loadFromFile(argv[++i]);
        } else if (std::strcmp(argv[i], "--log") == 0 && i + 1 < argc) {
            logPath = argv[++i];
        }
    }
}
