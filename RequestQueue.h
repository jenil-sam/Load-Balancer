/**
 * @file RequestQueue.h
 * @brief Declares the RequestQueue class, a queue of web Requests with firewall filtering.
 *
 * Wraps std::queue<Request> and adds IP range blocking for DOS/firewall protection.
 *
 * @author Jenil Sam
 * @date 2025
 */

#ifndef REQUESTQUEUE_H
#define REQUESTQUEUE_H

#include <queue>
#include <vector>
#include <string>
#include "Request.h"

/**
 * @brief A queue of Request objects with built-in IP firewall functionality.
 *
 * Maintains a FIFO queue of requests and allows blocking specific IP ranges
 * to prevent DOS attacks or restrict unwanted traffic.
 */
class RequestQueue {
public:
    /**
     * @brief Default constructor.
     */
    RequestQueue();

    /**
     * @brief Adds a request to the queue if its source IP is not blocked.
     * @param req The request to enqueue.
     * @return true if the request was added, false if blocked by firewall.
     */
    bool enqueue(const Request& req);

    /**
     * @brief Removes and returns the front request from the queue.
     * @return The next Request to be processed.
     */
    Request dequeue();

    /**
     * @brief Returns the number of requests currently in the queue.
     * @return Queue size.
     */
    int size() const;

    /**
     * @brief Checks if the queue is empty.
     * @return true if empty, false otherwise.
     */
    bool empty() const;

    /**
     * @brief Adds an IP prefix to the blocklist (e.g., "192.168." blocks all 192.168.x.x).
     * @param ipPrefix The IP prefix string to block.
     */
    void blockIPRange(const std::string& ipPrefix);

    /**
     * @brief Returns the list of currently blocked IP prefixes.
     * @return Reference to the vector of blocked IP prefixes.
     */
    const std::vector<std::string>& getBlockedRanges() const;

private:
    std::queue<Request> requests_;         ///< Underlying FIFO queue
    std::vector<std::string> blockedRanges_; ///< Blocked IP prefixes

    /**
     * @brief Checks if an IP address matches any blocked range.
     * @param ip The IP address to check.
     * @return true if the IP is blocked, false otherwise.
     */
    bool isBlocked(const std::string& ip) const;
};

#endif // REQUESTQUEUE_H
