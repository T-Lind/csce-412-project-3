/**
 * @file Request.h
 * @brief Request struct for LB simulation
 * @author Bizaco Load Balancer Project
 */

#ifndef REQUEST_H
#define REQUEST_H

#include <string>

/**
 * @struct Request
 * @brief A single web req with IPs, service time, and job type
 */
struct Request {
    std::string ipIn;     /**< incoming IP address */
    std::string ipOut;    /**< outgoing IP address */
    int serviceTime{0};    /**< Clock cycles required to process */
    char jobType{'P'};    /**< 'P' = Processing, 'S' = Streaming */
    int arrivalTime{0};    /**< Clock cycle when the req was created */
    int id{0};       /**< Unique req ID for logging */

    Request() = default;

    Request(std::string in, std::string out, int time, char type, int arrival, int reqId = 0)
        : ipIn(std::move(in)), ipOut(std::move(out)),
          serviceTime(time), jobType(type), arrivalTime(arrival), id(reqId) {}
};

#endif /* REQUEST_H */
