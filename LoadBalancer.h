/**
 * @file LoadBalancer.h
 * @brief Declares the LoadBalancer class that manages WebServers and the RequestQueue.
 *
 * The LoadBalancer maintains a pool of WebServers, distributes requests from
 * the queue, dynamically scales server count based on queue depth, and logs
 * all activity. Supports colored terminal output and IP-based firewall rules.
 *
 * @author Jenil Sam
 * @date 2025
 */

#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "WebServer.h"
#include "RequestQueue.h"
#include <vector>
#include <string>
#include <fstream>

/**
 * @brief ANSI color codes for terminal output.
 */
namespace Color {
    constexpr const char* RESET   = "\033[0m";
    constexpr const char* RED     = "\033[31m";
    constexpr const char* GREEN   = "\033[32m";
    constexpr const char* YELLOW  = "\033[33m";
    constexpr const char* CYAN    = "\033[36m";
    constexpr const char* MAGENTA = "\033[35m";
    constexpr const char* BOLD    = "\033[1m";
}

/**
 * @brief Manages web servers and request distribution with auto-scaling.
 *
 * Core responsibilities:
 * - Distributes requests from RequestQueue to idle WebServers each clock cycle
 * - Dynamically adds or removes servers to keep queue depth in [50*n, 80*n]
 * - Injects new random requests at configurable intervals
 * - Logs all events to a log file with colored terminal output
 */
class LoadBalancer {
public:
    /**
     * @brief Constructs a LoadBalancer.
     * @param name         Identifier for this load balancer instance.
     * @param numServers   Initial number of web servers.
     * @param totalCycles  Total simulation time in clock cycles.
     * @param logFile      Path to the output log file.
     * @param minQueue     Minimum queue threshold per server (default 50).
     * @param maxQueue     Maximum queue threshold per server (default 80).
     * @param scaleWait    Clock cycles to wait between scaling actions (default 100).
     * @param newReqInterval Cycles between injecting new random requests (default 10).
     * @param minServers   Minimum number of servers to keep running (default 2).
     */
    LoadBalancer(const std::string& name, int numServers, int totalCycles,
             const std::string& logFile,
             int minQueue = 50, int maxQueue = 80,
             int scaleWait = 100, int newReqInterval = 10, int minServers = 2);

    /**
     * @brief Destructor - closes log file.
     */
    ~LoadBalancer();

    /**
     * @brief Runs the full simulation for totalCycles_ clock cycles.
     */
    void run();

    /**
     * @brief Adds a blocked IP prefix to the firewall.
     * @param prefix IP prefix to block (e.g., "10.0.").
     */
    void blockIPRange(const std::string& prefix);

    /**
     * @brief Pre-fills the request queue with an initial batch of random requests.
     * @param count Number of initial requests to generate.
     */
    void populateInitialQueue(int count);

    /**
     * @brief Prints a final statistics summary to console and log.
     */
    void printSummary();

private:
    std::string name_;         
    int totalCycles_;           
    int currentCycle_;          //curr clock cycle number
    int minQueue_;            
    int maxQueue_;              //Queue max threshold per server
    int scaleWait_;             //Wait cycles between scaling events
    int newReqInterval_;        
    int lastScaleCycle_;        
    int totalRequestsServed_;   //Total requests completed across all servers
    int totalRequestsBlocked_;  //Total requests blocked by firewall
    int totalRequestsAdded_;    //Total requests injected during simulation
    int nextServerId_;          //server id to avoid duplication
    int minServers_;            //min number of servers

    std::vector<WebServer> servers_;  //web server pool
    RequestQueue queue_;              //request queue
    std::ofstream logStream_;         //Log file output

    /**
     * @brief Generates a random IPv4 address string.
     * @return Random IP address as a string.
     */
    std::string randomIP() const;

    /**
     * @brief Generates a random Request with random IPs, time, and job type.
     * @param minTime Minimum processing time (default 1).
     * @param maxTime Maximum processing time (default 20).
     * @return A new Request object.
     */
    Request generateRequest(int minTime = 1, int maxTime = 20) const;

    /**
     * @brief Distributes queue requests to idle servers.
     */
    void distributeRequests();

    /**
     * @brief Ticks all servers and collects completions.
     * @return Number of requests completed this cycle.
     */
    int tickServers();

    /**
     * @brief Checks queue depth and adds/removes servers as needed.
     */
    void autoScale();

    /**
     * @brief Adds one new WebServer to the pool.
     */
    void addServer();

    /**
     * @brief Removes one idle WebServer from the pool (if more than 1 exists).
     */
    void removeServer();

    /**
     * @brief Writes a colored log message to console and log file.
     * @param color ANSI color code string.
     * @param msg   Message to log.
     */
    void log(const char* color, const std::string& msg);
};

#endif // LOADBALANCER_H
