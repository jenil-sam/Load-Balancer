/**
 * @file Switch.cpp
 * @brief Implements the Switch class.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#include "Switch.h"
#include <cstdlib>
#include <sstream>
#include <iostream>

using namespace std;

Switch::Switch(int procServers, int streamServers, int cycles,
               const string& logPrefix)
    : processingLB_("ProcessingLB", procServers, cycles, logPrefix + "_processing.log"),
      streamingLB_("StreamingLB", streamServers, cycles, logPrefix + "_streaming.log"),
      routedToProcessing_(0), routedToStreaming_(0)
{}

string Switch::randomIP() const {
    ostringstream ss;
    ss << (rand() % 256) << "." << (rand() % 256) << "."
       << (rand() % 256) << "." << (rand() % 256);
    return ss.str();
}

Request Switch::generateRequest() const {
    string in  = randomIP();
    string out = randomIP();
    int t = 1 + rand() % 20;
    char type = (rand() % 2 == 0) ? 'P' : 'S';
    return Request(in, out, t, type);
}

void Switch::routeRequest(const Request& req) {
    if (req.jobType == 'P') routedToProcessing_++;
    else routedToStreaming_++;
}

void Switch::populateAndRoute(int total) {
    int forProcessing = 0, forStreaming = 0;
    for (int i = 0; i < total; ++i) {
        Request r = generateRequest();
        if (r.jobType == 'P') forProcessing++;
        else forStreaming++;
    }
    // Populate each LB with its share
    processingLB_.populateInitialQueue(forProcessing);
    streamingLB_.populateInitialQueue(forStreaming);
    routedToProcessing_ += forProcessing;
    routedToStreaming_   += forStreaming;

    cout << Color::CYAN
              << "[SWITCH] Routed " << forProcessing << " processing jobs -> ProcessingLB"
              << Color::RESET << "\n";
    cout << Color::CYAN
              << "[SWITCH] Routed " << forStreaming << " streaming jobs  -> StreamingLB"
              << Color::RESET << "\n";
}

void Switch::run() {
    cout << Color::BOLD
              << "\n-SWITCH- Running ProcessingLB...\n" << Color::RESET;
    processingLB_.run();

    cout << Color::BOLD
              << "\n-SWITCH- Running StreamingLB...\n" << Color::RESET;
    streamingLB_.run();
}
