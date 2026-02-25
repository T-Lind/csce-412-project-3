/**
 * @file IPBlocker.h
 * @brief IP range blocker for firewall / DOS prevention sim
 * @author Bizaco Load Balancer Project
 */

#ifndef IPBLOCKER_H
#define IPBLOCKER_H

#include <string>
#include <vector>

/**
 * @class IPBlocker
 * @brief Blocks IPs that fall within configured ranges
 */
class IPBlocker {
public:
    IPBlocker() = default;

    /**
     * Block a CIDR-style range (e.g. "192.168.0.0/16") OR precise IP (e.g. "10.0.0.1")
     * @param cidrOrIp CIDR string or single IP
     */
    void addBlockedRange(const std::string& cidrOrIp);

    /**
     * Check if the given IP is blocked
     * @param ip IPv4 address string
     * @return true if the IP is blocked
     */
    bool isBlocked(const std::string& ip) const;

    /**
     * Get list of blocked ranges for logging
     */
    const std::vector<std::string>& getBlockedRanges() const;

private:
    std::vector<std::string> blockedRanges_;

    /** Parse "a.b.c.d" or "a.b.c.d/n" and check if ip matches */
    static bool ipMatchesRange(const std::string& ip, const std::string& range);
    static unsigned int ipToInt(const std::string& ip);
};

#endif /* IPBLOCKER_H */
