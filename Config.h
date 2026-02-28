/**
 * @file Config.h
 * @brief Simple configuration file reader for the load balancer.
 *
 * Reads key=value pairs from a plain text config file.
 * This allows changing simulation parameters without recompiling.
 *
 * Example config file (loadbalancer.conf):
 * @code
 * servers=10
 * cycles=10000
 * min_queue=50
 * max_queue=80
 * scale_wait=100
 * new_req_interval=10
 * block_ip=10.0.
 * block_ip=192.168.1.
 * use_switch=0
 * log_file=simulation.log
 * @endcode
 *
 * @author Jenil Sam
 * @date 2025
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * @brief Reads and stores configuration parameters from a key=value file.
 */
class Config
{
public:
    /**
     * @brief Constructs Config with default values.
     */
    Config()
        : servers(10), cycles(10000), minQueue(50), maxQueue(80),
          scaleWait(100), newReqInterval(10), useSwitch(false),
          logFile("simulation.log"), minServers(2) {}

    int servers;
    int cycles;
    int minQueue;
    int maxQueue;
    int scaleWait;
    int newReqInterval;
    bool useSwitch;
    int minServers;
    std::string logFile;
    std::vector<std::string> blockedIPs;

    /**
     * @brief Loads configuration from a file. Missing keys use defaults.
     * @param filename Path to the configuration file.
     * @return true if file was opened and read; false if file not found.
     */
    bool load(const std::string &filename)
    {
        std::ifstream f(filename);
        if (!f.is_open())
        {
            std::cerr << "Config file not found: " << filename
                      << " -- using defaults.\n";
            return false;
        }
        std::string line;
        while (std::getline(f, line))
        {
            if (line.empty() || line[0] == '#')
                continue;
            auto pos = line.find('=');
            if (pos == std::string::npos)
                continue;
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            val.erase(0, val.find_first_not_of(" \t"));
            val.erase(val.find_last_not_of(" \t") + 1);

            if (key == "servers")
                servers = std::stoi(val);
            else if (key == "cycles")
                cycles = std::stoi(val);
            else if (key == "min_queue")
                minQueue = std::stoi(val);
            else if (key == "max_queue")
                maxQueue = std::stoi(val);
            else if (key == "scale_wait")
                scaleWait = std::stoi(val);
            else if (key == "new_req_interval")
                newReqInterval = std::stoi(val);
            else if (key == "use_switch")
                useSwitch = (val == "1" || val == "true");
            else if (key == "log_file")
                logFile = val;
            else if (key == "block_ip")
                blockedIPs.push_back(val);
        }
        return true;
    }

    /**
     * @brief Prints current configuration values to stdout.
     */
    void print() const
    {
        std::cout << "=== Configuration ===\n"
                  << "  servers         = " << servers << "\n"
                  << "  cycles          = " << cycles << "\n"
                  << "  min_queue       = " << minQueue << "\n"
                  << "  max_queue       = " << maxQueue << "\n"
                  << "  scale_wait      = " << scaleWait << "\n"
                  << "  new_req_interval= " << newReqInterval << "\n"
                  << "  use_switch      = " << useSwitch << "\n"
                  << "  log_file        = " << logFile << "\n";
        for (const auto &ip : blockedIPs)
            std::cout << "  block_ip        = " << ip << "\n";
        std::cout << "=====================\n\n";
    }
};

#endif // CONFIG_H
