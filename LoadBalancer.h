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

using namespace std;

/**
 * @brief ANSI color codes for terminal output.
 */
namespace Color {
    constexpr const char* RESET   = "\033[0m";  ///< Reset all attributes
    constexpr const char* RED     = "\033[31m"; ///< Red color for errors
    constexpr const char* GREEN   = "\033[32m"; ///< Green color for scale-up events
    constexpr const char* YELLOW  = "\033[33m"; ///< Yellow color for firewall events
    constexpr const char* CYAN    = "\033[36m"; ///< Cyan color for status messages
    constexpr const char* MAGENTA = "\033[35m"; ///< Magenta color for scale-down events
    constexpr const char* BOLD    = "\033[1m";  ///< Bold text for headers and summaries
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
     * @param name           Identifier for this load balancer instance.
     * @param numServers     Initial number of web servers.
     * @param totalCycles    Total simulation time in clock cycles.
     * @param logFile        Path to the output log file.
     * @param minQueue       Minimum queue threshold per server (default 50).
     * @param maxQueue       Maximum queue threshold per server (default 80).
     * @param scaleWait      Clock cycles to wait between scaling actions (default 100).
     * @param newReqInterval Cycles between injecting new random requests (default 10).
     * @param minServers     Minimum number of servers to keep running (default 2).
     */
    LoadBalancer(const string& name, int numServers, int totalCycles,
                 const string& logFile,
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
    void blockIPRange(const string& prefix);

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
    string name_;          ///< Name of this load balancer
    int totalCycles_;           ///< Total simulation clock cycles
    int currentCycle_;          ///< Current clock cycle number
    int minQueue_;              ///< Queue min threshold per server
    int maxQueue_;              ///< Queue max threshold per server
    int scaleWait_;             ///< Wait cycles between scaling events
    int newReqInterval_;        ///< Cycles between new request injections
    int lastScaleCycle_;        ///< Last cycle a scale event occurred
    int totalRequestsServed_;   ///< Total requests completed across all servers
    int totalRequestsBlocked_;  ///< Total requests blocked by firewall
    int totalRequestsAdded_;    ///< Total requests injected during simulation
    int nextServerId_;          ///< Next server ID to assign (avoids duplicate IDs)
    int minServers_;            ///< Minimum number of servers to keep running
    int scaleUpCount_;          ///< Total number of scale-up events
    int scaleDownCount_;        ///< Total number of scale-down events
    int minTaskTime_;           ///< Minimum task processing time in cycles
    int maxTaskTime_;           ///< Maximum task processing time in cycles
    int initialServers_;        ///< Initial number of servers at simulation start

    vector<WebServer> servers_;  ///< Active web server pool
    RequestQueue queue_;              ///< The request queue
    ofstream logStream_;         ///< Log file output stream

    /**
     * @brief Generates a random IPv4 address string.
     * @return Random IP address as a string.
     */
    string randomIP() const;

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
     * @brief Removes one idle WebServer from the pool (if more than minServers_ exists).
     */
    void removeServer();

    /**
     * @brief Writes a colored log message to console and log file.
     * @param color ANSI color code string.
     * @param msg   Message to log.
     */
    void log(const char* color, const string& msg);
};

#endif // LOADBALANCER_H