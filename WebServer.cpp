/**
 * @file WebServer.cpp
 * @brief Implements the WebServer class.
 *
 * @author Jenil Sam
 * @date 2025
 */

#include "WebServer.h"

WebServer::WebServer(int id)
    : id_(id), idle_(true), remainingTime_(0), completedCount_(0) {}

void WebServer::assignRequest(const Request& req) {
    currentRequest_ = req;
    remainingTime_ = req.time;
    idle_ = false;
}

bool WebServer::tick() {
    if (idle_) return false;
    remainingTime_--;
    if (remainingTime_ <= 0) {
        idle_ = true;
        completedCount_++;
        return true; // job just finished
    }
    return false;
}

bool WebServer::isIdle() const {
    return idle_;
}

int WebServer::getId() const {
    return id_;
}

int WebServer::getCompletedCount() const {
    return completedCount_;
}

const Request& WebServer::getCurrentRequest() const {
    return currentRequest_;
}
