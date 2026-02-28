/**
 * @file Switch.h
 * @brief Declares the Switch class that routes requests by job type to separate LoadBalancers.
 *
 * The Switch acts as a higher-level router, directing Processing ('P') jobs to one
 * LoadBalancer and Streaming ('S') jobs to another. This is the bonus implementation.
 *
 * @author Jenil Sam
 * @date 2025
 */

#ifndef SWITCH_H
#define SWITCH_H

#include "LoadBalancer.h"
#include "Request.h"
#include <memory>
#include <string>

/**
 * @brief Routes requests based on job type to dedicated LoadBalancer instances.
 *
 * Holds two LoadBalancer objects:
 * - processingLB_: Handles 'P' (Processing) type requests
 * - streamingLB_:  Handles 'S' (Streaming) type requests
 *
 * Generates random requests and routes them accordingly before running both simulations.
 */
class Switch {
public:
    /**
     * @brief Constructs a Switch with two load balancers.
     * @param procServers   Initial server count for processing load balancer.
     * @param streamServers Initial server count for streaming load balancer.
     * @param cycles        Total simulation duration in clock cycles.
     * @param logPrefix     Prefix for log file names.
     */
    Switch(int procServers, int streamServers, int cycles,
           const std::string& logPrefix = "switch");

    /**
     * @brief Routes a request to the appropriate load balancer by job type.
     * @param req The request to route.
     */
    void routeRequest(const Request& req);

    /**
     * @brief Generates and routes an initial batch of requests.
     * @param total Total number of requests to generate and distribute.
     */
    void populateAndRoute(int total);

    /**
     * @brief Runs both load balancers sequentially.
     */
    void run();

private:
    LoadBalancer processingLB_; // Load balancer for processing jobs
    LoadBalancer streamingLB_;  // Load balancer for streaming jobs
    int routedToProcessing_;    // Number requests to processing LB
    int routedToStreaming_;     // NUmebr requests to streaming LB

    /**
     * @brief Generates a random IPv4 address.
     * @return IP as string.
     */
    std::string randomIP() const;

    /**
     * @brief Generates a random Request.
     * @return New Request.
     */
    Request generateRequest() const;
};

#endif // SWITCH_H
