/**
 * @file Request.h
 * @brief Defines the Request structure for web requests processed by the load balancer.
 *
 * Each request contains source/destination IP addresses, processing time,
 * and job type (Processing or Streaming).
 *
 * @author Jenil Sam
 * @date 2025
 */

#ifndef REQUEST_H
#define REQUEST_H

#include <string>

/**
 * @brief Represents a single web request in the load balancer system.
 *
 * Contains all metadata needed for routing and processing:
 * - Source and destination IP addresses
 * - Estimated processing time in clock cycles
 * - Job type: 'P' for Processing, 'S' for Streaming
 */
struct Request {
    std::string ipIn;    ///< Source IP address of the request
    std::string ipOut;   ///< Destination IP address for the response
    int time;            ///< Processing time required (in clock cycles)
    char jobType;        ///< Job type: 'P' = Processing, 'S' = Streaming

    /**
     * @brief Default constructor - creates an empty request.
     */
    Request() : ipIn(""), ipOut(""), time(0), jobType('P') {}

    /**
     * @brief Parameterized constructor.
     * @param in Source IP address
     * @param out Destination IP address
     * @param t Processing time in clock cycles
     * @param type Job type character ('P' or 'S')
     */
    Request(const std::string& in, const std::string& out, int t, char type)
        : ipIn(in), ipOut(out), time(t), jobType(type) {}
};

#endif // REQUEST_H
