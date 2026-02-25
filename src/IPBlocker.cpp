/**
 * @file IPBlocker.cpp
 * @brief Implementation of IPBlocker.
 */

#include "IPBlocker.h"
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <cctype>

namespace {

std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end == std::string::npos ? std::string::npos : end - start + 1);
}

} // namespace

void IPBlocker::addBlockedRange(const std::string& cidrOrIp) {
    std::string s = trim(cidrOrIp);
    if (!s.empty()) blockedRanges_.push_back(s);
}

bool IPBlocker::isBlocked(const std::string& ip) const {
    for (const auto& range : blockedRanges_) {
        if (ipMatchesRange(ip, range)) return true;
    }
    return false;
}

const std::vector<std::string>& IPBlocker::getBlockedRanges() const {
    return blockedRanges_;
}



bool IPBlocker::ipMatchesRange(const std::string& ip, const std::string& range) {
    size_t slash = range.find('/');
    if (slash == std::string::npos) {
        return ip == range;
    }
    std::string baseIp = range.substr(0, slash);
    std::string prefixLenStr = range.substr(slash + 1);
    int prefixLen = 32;
    try {
        prefixLen = std::stoi(prefixLenStr);
    } catch (...) {
        return false;
    }
    if (prefixLen < 0 || prefixLen > 32) return false;
    unsigned int ipVal = ipToInt(ip);
    unsigned int baseVal = ipToInt(baseIp);
    unsigned int mask = prefixLen == 0 ? 0 : (0xFFFFFFFFu << (32 - prefixLen));
    return (ipVal & mask) == (baseVal & mask);
}


unsigned int IPBlocker::ipToInt(const std::string& ip) {
    unsigned int result = 0;
    std::string s = ip;
    for (int i = 0; i < 4; ++i) {
        size_t dot = s.find('.');
        std::string part = dot == std::string::npos ? s : s.substr(0, dot);
        try { int octet = std::stoi(part);
            if (octet < 0 || octet > 255) { return 0;}
            result = (result << 8) | (unsigned int)(octet & 0xFF);
        } catch (...) { return 0;}
        if (dot == std::string::npos) { break;}
        s = s.substr(dot + 1);
    }
    return result;
}
