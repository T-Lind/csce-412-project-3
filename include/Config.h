/**
 * @file Config.h
 * @brief Load balancer simulation configuration.
 * @author Bizaco Load Balancer Project
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <cstddef>

/**
 * @struct Config
 * @brief Holds all configurable parameters for the LB simulation
 */
struct Config {
    int initialServers{10};
    int runTime{10000};
    int scaleCooldown{50};
    int initialQueueSize{1000};   /**< typically servers * 100 */
    int lowFactor{50};            /**< Scale down if queue < lowFactor * servers */
    int highFactor{80};          /**< Scale up if queue > highFactor * servers */
    int minServiceTime{1};
    int maxServiceTime{50};
    int newRequestProbabilityPercent{5};  /**< per-cycle probability of adding a new req (0-100) */
    unsigned int seed{0};         /**< 0 = use time-based seed */
    std::string configPath;
    std::string logPath;
    std::vector<std::string> blockedRanges;  /**< IP or CIDR ranges to block */

    /**
     * Load configuration from a file (key=value, one per line)
     * @param path Path to config file
     * @return true if file was read (missing file leaves defaults)
     */
    bool loadFromFile(const std::string& path);

    /**
     * Apply command-line overrides (for example, --servers 10 --runtime 10000)
     * @param argc Argument count
     * @param argv Argument vector
     */
    void applyCommandLine(int argc, char* argv[]);
};

#endif /* CONFIG_H */
