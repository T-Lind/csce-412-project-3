/**
 * @file WebServer.h
 * @brief Single web server that processes one request at a time
 * @author Bizaco Load Balancer Project
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Request.h"

/**
 * @class WebServer
 * @brief Handles one request at a time; tracks state until completion
 */
class WebServer {
public:
    /**
     * @param id unique server identifier
     */
    explicit WebServer(int id);

    /**
     * if server is still processing a req at the given time
     */
    bool isBusy(int currentTime) const;

    /**
     * assign a req to the server
     */
    void assignRequest(const Request& r, int currentTime);

    /**
     * adv. server state
     */
    void tick(int currentTime);

    int getId() const;

    /**
     * if server is not deallocated
     */
    bool active() const;
    void setActive(bool a);

    /**
     * Req currently being procesed
     */
    const Request* currentRequest() const;

    /**
     * Set current request as completed; cna accept new work
     */
    void markCompleted();

private:
    int id_;
    int bU_{-1};
    bool active_{true};
    Request cR_;
};

#endif /* WEBSERVER_H */
