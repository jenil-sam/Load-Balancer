/**
 * @file RequestQueue.cpp
 * @brief Implements the RequestQueue class.
 *
 * @author Jenil Sam
 * @date 2025
 */

#include "RequestQueue.h"
#include <algorithm>

RequestQueue::RequestQueue() {}

bool RequestQueue::enqueue(const Request& req) {
    if (isBlocked(req.ipIn)) {
        return false;
    }
    requests_.push(req);
    return true;
}

Request RequestQueue::dequeue() {
    Request req = requests_.front();
    requests_.pop();
    return req;
}

int RequestQueue::size() const {
    return static_cast<int>(requests_.size());
}

bool RequestQueue::empty() const {
    return requests_.empty();
}

void RequestQueue::blockIPRange(const std::string& ipPrefix) {
    blockedRanges_.push_back(ipPrefix);
}

const std::vector<std::string>& RequestQueue::getBlockedRanges() const {
    return blockedRanges_;
}

bool RequestQueue::isBlocked(const std::string& ip) const {
    for (const auto& prefix : blockedRanges_) {
        if (ip.substr(0, prefix.size()) == prefix) {
            return true;
        }
    }
    return false;
}
