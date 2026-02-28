/**
 * @file main.cpp
 * @brief Driver program for the Load Balancer simulation.
 *
 * Reads configuration from loadbalancer.conf (or uses defaults/user input),
 * creates the LoadBalancer or Switch, populates the initial queue,
 * runs the simulation, and produces a summary log.
 *
 * @author Jenil Sam
 * @date 2025
 */

/**
 * @mainpage Load Balancer Simulation
 *
 * @section overview Overview
 * This project simulates a production-grade load balancer that distributes
 * incoming web requests across a dynamic pool of web servers.
 *
 * @section features Features
 * - Auto-scaling based on queue depth thresholds (50*n and 80*n)
 * - IP-based firewall / DOS prevention
 * - Config file support (no recompile needed)
 * - Colored terminal output
 * - Detailed simulation log
 * - Bonus: Switch routing by job type (Processing vs Streaming)
 *
 * @section usage How to Run
 * @code
 * make
 * ./loadbalancer
 * @endcode
 *
 * @section files File Structure
 * - main.cpp — Driver program
 * - LoadBalancer.h/cpp — Core balancer with auto-scaling
 * - WebServer.h/cpp — Individual server simulation
 * - RequestQueue.h/cpp — FIFO queue with firewall filtering
 * - Request.h — Request data structure
 * - Switch.h/cpp — Bonus job-type router
 * - Config.h — Config file parser
 *
 * @author Jenil Sam
 * @date 2025
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include "Config.h"
#include "LoadBalancer.h"
#include "Switch.h"

/**
 * @brief Prints usage information.
 * @param prog Program name.
 */
void printUsage(const char* prog) {
    std::cout << "Usage: " << prog << " [config_file]\n"
              << "  config_file  Optional path to config file (default: loadbalancer.conf)\n"
              << "  If no config file found, user is prompted for key values.\n\n";
}

/**
 * @brief Prompts user for simulation parameters if config file is not found.
 * @param cfg Config object to populate.
 */
void promptUser(Config& cfg) {
    std::cout << Color::CYAN << "=== Load Balancer Setup ===" << Color::RESET << "\n";
    std::cout << "Enter number of initial web servers [" << cfg.servers << "]: ";
    std::string s;
    std::getline(std::cin, s);
    if (!s.empty()) cfg.servers = std::stoi(s);

    std::cout << "Enter total clock cycles to simulate [" << cfg.cycles << "]: ";
    std::getline(std::cin, s);
    if (!s.empty()) cfg.cycles = std::stoi(s);

    std::cout << "Use Switch (route by job type)? (1=yes, 0=no) [0]: ";
    std::getline(std::cin, s);
    if (!s.empty()) cfg.useSwitch = (s == "1");
}

/**
 * @brief Main entry point.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit code.
 */
int main(int argc, char* argv[]) {
    srand(static_cast<unsigned>(time(nullptr)));

    std::cout << Color::BOLD << Color::CYAN
              << "---------Load Balancer--------- \n"
              << Color::RESET << "\n";

    // Load configuration
    Config cfg;
    std::string configFile = (argc > 1) ? argv[1] : "loadbalancer.conf";
    bool loaded = cfg.load(configFile);
    if (!loaded) {
        promptUser(cfg);
    }
    cfg.print();

    //queue size
    int initialQueueSize = cfg.servers * 100;

    if (cfg.useSwitch) {
        int procServers   = cfg.servers / 2 > 0 ? cfg.servers / 2 : 1;
        int streamServers = cfg.servers - procServers;
        Switch sw(procServers, streamServers, cfg.cycles, "switch");
        for ([[maybe_unused]] const auto& ip : cfg.blockedIPs) {
        }
        sw.populateAndRoute(initialQueueSize);
        sw.run();
    } else {
        LoadBalancer lb("MainLB", cfg.servers, cfg.cycles, cfg.logFile,
                        cfg.minQueue, cfg.maxQueue,
                        cfg.scaleWait, cfg.newReqInterval);

        for ([[maybe_unused]] const auto& ip : cfg.blockedIPs) {
            lb.blockIPRange(ip);
        }
        //initial queue
        lb.populateInitialQueue(initialQueueSize);

        lb.run();
    }

    std::cout << Color::GREEN
              << "\nSimulation complete. See log file for details.\n"
              << Color::RESET;
    return 0;
}
