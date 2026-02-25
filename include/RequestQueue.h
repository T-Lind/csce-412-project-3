/**
 * @file RequestQueue.h
 * @brief Queue of Request objects for the LB
 * @author Bizaco Load Balancer Project
 */

#ifndef REQUESTQUEUE_H
#define REQUESTQUEUE_H

#include "Request.h"
#include <queue>
#include <cstddef>

/**
 * @class RequestQueue
 * @brief Wrapper around std::queue<Request> for the LB
 */
class RequestQueue {
public:
    RequestQueue() = default;

    /**
     * Add a req to the back of the queue
     * @param r req to enqueue
     */
    void enqueue(const Request& r);

    /**
     * Remove and return the front req if the queue is not empty
     * @param out req to fill with front value
     * @return true if a req was dequeued, false if queue was empty
     */
    bool try_dequeue(Request& out);

    /**
     * Number of reqs currently in the queue
     */
    size_t size() const;

    /**
     * Check if the queue is empty
     */
    bool empty() const;

private:
    std::queue<Request> queue_;
};

#endif /* REQUESTQUEUE_H */
