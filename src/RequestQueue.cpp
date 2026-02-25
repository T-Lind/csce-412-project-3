/**
 * @file RequestQueue.cpp
 * @brief Implementation of RequestQueue.
 */

#include "RequestQueue.h"

void RequestQueue::enqueue(const Request& r) {
    queue_.push(r);
}

bool RequestQueue::try_dequeue(Request& out) {
    if (queue_.empty()) return false;
    out = queue_.front();
    queue_.pop();
    return true;
}

size_t RequestQueue::size() const {
    return queue_.size();
}

bool RequestQueue::empty() const {
    return queue_.empty();
}
