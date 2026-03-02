/**
 * @file WebServer.h
 * @brief Declares the WebServer class representing a single web server instance.
 *
 * Each WebServer processes one Request at a time, tracking remaining
 * clock cycles until the current job completes.
 *
 * @author Jenil Sam
 * @date 2025
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Request.h"
#include <string>

/**
 * @brief Simulates a single web server that processes requests.
 *
 * A WebServer can be idle or busy. When busy, it counts down clock cycles
 * until the current request is complete, then signals it is ready for another.
 */
class WebServer {
public:
    /**
     * @brief Constructs a WebServer with a given ID.
     * @param id Unique identifier for this server.
     */
    explicit WebServer(int id);

    /**
     * @brief Assigns a new request to this server for processing.
     * @param req The request to process.
     */
    void assignRequest(const Request& req);

    /**
     * @brief Advances the server's internal clock by one cycle.
     *        Decrements remaining processing time if busy.
     * @return true if the server just finished a job this cycle.
     */
    bool tick();

    /**
     * @brief Checks if the server is currently idle (no active request).
     * @return true if idle, false if processing a request.
     */
    bool isIdle() const;

    /**
     * @brief Returns the server's unique ID.
     * @return Integer ID.
     */
    int getId() const;

    /**
     * @brief Returns the total number of requests this server has completed.
     * @return Completed request count.
     */
    int getCompletedCount() const;

    /**
     * @brief Returns the currently assigned request (valid only when busy).
     * @return Current Request being processed.
     */
    const Request& getCurrentRequest() const;

private:
    int id_;                 ///< Unique identifier for this server
    bool idle_;              ///< True if server is idle, false if processing
    int remainingTime_;      ///< Remaining clock cycles for current request
    int completedCount_;     ///< Total number of requests completed
    Request currentRequest_; ///< The request currently being processed
};

#endif // WEBSERVER_H
