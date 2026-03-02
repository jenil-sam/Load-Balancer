/**
 * @file LoadBalancer.cpp
 * @brief Implements the LoadBalancer class.
 *
 * @author Jenil Sam
 * @date 2025
 */

#include "LoadBalancer.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <iomanip>

LoadBalancer::LoadBalancer(const std::string &name, int numServers, int totalCycles,
                           const std::string &logFile,
                           int minQueue, int maxQueue,
                           int scaleWait, int newReqInterval, int minServers)
    : name_(name), totalCycles_(totalCycles), currentCycle_(0),
      minQueue_(minQueue), maxQueue_(maxQueue),
      scaleWait_(scaleWait), newReqInterval_(newReqInterval),
      lastScaleCycle_(0), totalRequestsServed_(0),
      totalRequestsBlocked_(0), totalRequestsAdded_(0),
      nextServerId_(numServers + 1), minServers_(minServers),
      scaleUpCount_(0), scaleDownCount_(0),
      minTaskTime_(1), maxTaskTime_(20),
      initialServers_(numServers)
{
    for (int i = 0; i < numServers; ++i)
    {
        servers_.emplace_back(i + 1);
    }
    logStream_.open(logFile);
    if (!logStream_.is_open())
    {
        std::cerr << Color::RED << "[ERROR] cannot open log file: " << logFile << Color::RESET << "\n";
    }
}

LoadBalancer::~LoadBalancer()
{
    if (logStream_.is_open())
        logStream_.close();
}

std::string LoadBalancer::randomIP() const
{
    std::ostringstream ss;
    ss << (rand() % 256) << "." << (rand() % 256) << "."
       << (rand() % 256) << "." << (rand() % 256);
    return ss.str();
}

Request LoadBalancer::generateRequest(int minTime, int maxTime) const
{
    std::string in = randomIP();
    std::string out = randomIP();
    int t = minTime + rand() % (maxTime - minTime + 1);
    char type = (rand() % 2 == 0) ? 'P' : 'S';
    return Request(in, out, t, type);
}

void LoadBalancer::blockIPRange(const std::string &prefix)
{
    queue_.blockIPRange(prefix);
    log(Color::YELLOW, "[FIREWALL] Blocking IP range: " + prefix + "x.x");
}

void LoadBalancer::populateInitialQueue(int count)
{
    int blocked = 0;
    for (int i = 0; i < count; ++i)
    {
        Request r = generateRequest();
        if (!queue_.enqueue(r))
            blocked++;
    }
    std::ostringstream ss;
    ss << "[INIT] " << name_ << " populated queue with " << (count - blocked)
       << " requests (" << blocked << " blocked). Queue size: " << queue_.size();
    log(Color::CYAN, ss.str());
    totalRequestsBlocked_ += blocked;
    totalRequestsAdded_ += count - blocked;
}

void LoadBalancer::log(const char *color, const std::string &msg)
{
    std::cout << color << msg << Color::RESET << "\n";
    if (logStream_.is_open())
    {
        logStream_ << msg << "\n";
    }
}

void LoadBalancer::distributeRequests()
{
    for (auto &server : servers_)
    {
        if (server.isIdle() && !queue_.empty())
        {
            Request r = queue_.dequeue();
            server.assignRequest(r);
        }
    }
}

int LoadBalancer::tickServers()
{
    int completed = 0;
    for (auto &server : servers_)
    {
        if (server.tick())
        {
            completed++;
            totalRequestsServed_++;
        }
    }
    return completed;
}

void LoadBalancer::autoScale()
{
    if ((currentCycle_ - lastScaleCycle_) < scaleWait_)
        return;

    int n = static_cast<int>(servers_.size());
    int qSize = queue_.size();

    if (qSize > maxQueue_ * n)
    {
        addServer();
        lastScaleCycle_ = currentCycle_;
    }
    else if (qSize < minQueue_ * n && n > minServers_)
    {
        removeServer();
        lastScaleCycle_ = currentCycle_;
    }
}

void LoadBalancer::addServer()
{
    int newId = nextServerId_++;
    servers_.emplace_back(newId);
    std::ostringstream ss;
    ss << Color::GREEN << "[SCALE+] " << name_ << " Cycle " << currentCycle_
       << ": Added server #" << newId
       << ". Total servers: " << servers_.size()
       << ". Queue: " << queue_.size() << Color::RESET;
    scaleUpCount_++;
    log(Color::GREEN, ss.str());
}

void LoadBalancer::removeServer()
{
    // Find last idle server and remove it
    for (int i = static_cast<int>(servers_.size()) - 1; i >= 0; --i)
    {
        if (servers_[i].isIdle())
        {
            int removedId = servers_[i].getId();
            servers_.erase(servers_.begin() + i);
            std::ostringstream ss;
            ss << "[SCALE-] " << name_ << " Cycle " << currentCycle_
               << ": Removed idle server #" << removedId
               << ". Total servers: " << servers_.size()
               << ". Queue: " << queue_.size();
            scaleDownCount_++;
            log(Color::MAGENTA, ss.str());
            return;
        }
    }
}

void LoadBalancer::run()
{
    std::ostringstream header;
    header << "\n"
           << std::string(60, '=') << "\n"
           << "  " << name_ << " -- Starting simulation\n"
           << "  Servers: " << servers_.size()
           << "  Duration: " << totalCycles_ << " cycles\n"
           << "  Queue thresholds: [" << minQueue_ << "*n, " << maxQueue_ << "*n]\n"
           << std::string(60, '=');
    log(Color::BOLD, header.str());

    for (currentCycle_ = 1; currentCycle_ <= totalCycles_; ++currentCycle_)
    {

        // Add new requests periodically
        if (currentCycle_ % newReqInterval_ == 0)
        {
            int newCount = 1 + rand() % 5;
            for (int i = 0; i < newCount; ++i)
            {
                Request r = generateRequest();
                if (!queue_.enqueue(r))
                {
                    totalRequestsBlocked_++;
                }
                else
                {
                    totalRequestsAdded_++;
                }
            }
        }

        distributeRequests();
        tickServers();
        autoScale();

        // Periodic status log every 500 cycles
        if (currentCycle_ % 500 == 0)
        {
            std::ostringstream ss;
            ss << "[STATUS] " << name_ << " Cycle " << std::setw(6) << currentCycle_
               << " | Queue: " << std::setw(5) << queue_.size()
               << " | Servers: " << std::setw(3) << servers_.size()
               << " | Served so far: " << totalRequestsServed_;
            log(Color::CYAN, ss.str());
        }
    }

    printSummary();
}

void LoadBalancer::printSummary()
{
    std::ostringstream ss;
    ss << "\n"
       << std::string(60, '=') << "\n"
       << "  SUMMARY: " << name_ << "\n"
       << std::string(60, '-') << "\n"
       << "  Total clock cycles:       " << totalCycles_ << "\n"
       << "  Initial server count:     " << initialServers_ << "\n"
       << "  Final server count:       " << servers_.size() << "\n"
       << "  Starting queue depth:     " << (initialServers_ * 100) << "\n"
       << "  Remaining queue depth:    " << queue_.size() << "\n"
       << "  Task time range:          " << minTaskTime_ << " - " << maxTaskTime_ << " cycles\n"
       << "  Total requests added:     " << totalRequestsAdded_ << "\n"
       << "  Total requests served:    " << totalRequestsServed_ << "\n"
       << "  Total requests blocked:   " << totalRequestsBlocked_ << "\n"
       << "  Scale-up events:          " << scaleUpCount_ << "\n"
       << "  Scale-down events:        " << scaleDownCount_ << "\n"
       << "  Throughput rate:          " << std::fixed << std::setprecision(2)
       << (static_cast<double>(totalRequestsServed_) / totalCycles_) << " req/cycle\n"
       << std::string(60, '-') << "\n"
       << "  Per-server completions:\n";
    for (const auto &s : servers_)
    {
        ss << "    Server #" << s.getId()
           << ": " << s.getCompletedCount() << " requests\n";
    }
    ss << std::string(60, '=');
    log(Color::BOLD, ss.str());
}