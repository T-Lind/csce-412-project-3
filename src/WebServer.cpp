/**
 * @file WebServer.cpp
 * @brief Implementation of WebServer.
 */

#include "WebServer.h"

WebServer::WebServer(int id) : id_(id) {}

bool WebServer::isBusy(int currentTime) const {
    return active_ && bU_ > currentTime;
}

void WebServer::assignRequest(const Request& r, int currentTime) {
    cR_ = r;
    bU_ = currentTime + r.serviceTime;
}

void WebServer::tick(int currentTime) {
    (void)currentTime;
}

int WebServer::getId() const {
    return id_;
}

bool WebServer::active() const {
    return active_;
}

void WebServer::setActive(bool a) {
    active_ = a;
}

const Request* WebServer::currentRequest() const {
    if (bU_ < 0) return nullptr;
    return &cR_;
}

void WebServer::markCompleted() {
    bU_ = -1;
}
