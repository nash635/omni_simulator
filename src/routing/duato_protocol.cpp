#include "duato_protocol.h"
#include "network/network.h"
#include "network/node.h"
#include "message/message.h"
#include "utils/config.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <climits>
#include <string>

DuatoProtocol::DuatoProtocol() : network(nullptr), config(nullptr) {}

DuatoProtocol::DuatoProtocol(Network* network) : network(network), config(nullptr) {}

DuatoProtocol::DuatoProtocol(Network* network, const Config* config) 
    : network(network), config(config) {}

DuatoProtocol::~DuatoProtocol() {}

RoutingResult DuatoProtocol::routeMessageWithStats(Message& message, Node* source, Node* destination) {
    RoutingResult result;
    result.success = false;
    result.hopCount = 0;
    result.totalDelay = 0.0;
    
    std::vector<Node*> path = findPath(source, destination);
    
    if (path.empty()) {
        debugDeadlock(source, "No path found to destination");
        return result;
    }
    
    result.path = path;
    result.hopCount = path.size() - 1;
    result.totalDelay = calculatePathDelay(path);
    result.success = true;
    
    debugLog("Path found with " + std::to_string(result.hopCount) + 
             " hops, total delay: " + std::to_string(result.totalDelay) + " cycles");
    
    return result;
}

std::vector<Node*> DuatoProtocol::findPath(Node* source, Node* destination) const {
    std::vector<Node*> path;
    Node* current = source;
    path.push_back(current);
    
    while (current != destination) {
        Link* nextLink = nullptr;
        
        if (canUseAdaptiveChannel(current, nullptr, destination)) {
            nextLink = selectAdaptiveRoute(current, destination);
            if (nextLink && !isChannelFree(nextLink, VirtualChannel::ADAPTIVE)) {
                nextLink = nullptr;
            }
        }
        
        if (!nextLink) {
            nextLink = getDeterministicRoute(current, destination);
            if (nextLink && !isChannelFree(nextLink, VirtualChannel::DETERMINISTIC)) {
                return std::vector<Node*>();
            }
        }
        
        if (!nextLink) {
            return std::vector<Node*>();
        }
        
        Node* next = (nextLink->getNodeA() == current) ? 
                    nextLink->getNodeB() : nextLink->getNodeA();
        
        path.push_back(next);
        current = next;
        
        if (path.size() > 100) {
            debugLog("Path too long, possible routing loop detected");
            return std::vector<Node*>();
        }
    }
    
    return path;
}

double DuatoProtocol::calculatePathDelay(const std::vector<Node*>& path) const {
    if (path.size() < 2) {
        return 0.0;
    }
    
    double totalDelay = 0.0;
    double linkLatency = (config) ? config->getLinkLatency() : 1.0;
    
    double transmissionDelay = (path.size() - 1) * linkLatency;
    
    double queuingDelay = 0.0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        queuingDelay += 0.5;
    }
    
    totalDelay = transmissionDelay + queuingDelay;
    
    return totalDelay;
}

void DuatoProtocol::routeMessage(Message& message, Node* source, Node* destination) {
    RoutingResult result = routeMessageWithStats(message, source, destination);
}

void DuatoProtocol::debugLog(const std::string& message) const {
    if (config && config->isRoutingVerbose()) {
        std::cout << "[ROUTING] " << message << std::endl;
    }
}

void DuatoProtocol::debugRouting(const std::string& action, Node* current, Node* next, const std::string& channel) const {
    if (config && config->isRoutingVerbose()) {
        std::cout << "[ROUTING] " << action << " via " << channel << " channel from (" 
                  << current->getX() << "," << current->getY() << ") to (" 
                  << next->getX() << "," << next->getY() << ")" << std::endl;
    }
}

void DuatoProtocol::debugDeadlock(Node* current, const std::string& reason) const {
    if (config && config->isDeadlockDetectionVerbose()) {
        std::cout << "[DEADLOCK] " << reason << " at (" << current->getX() 
                  << "," << current->getY() << ")" << std::endl;
    }
}

bool DuatoProtocol::canUseAdaptiveChannel(Node* current, Node* next, Node* destination) const {
    if (!next) return true;
    return isMinimalRoute(current, next, destination);
}

bool DuatoProtocol::mustUseDeterministicChannel(Node* current, Node* destination) const {
    return calculateManhattanDistance(current, destination) <= 2;
}

Link* DuatoProtocol::selectAdaptiveRoute(Node* current, Node* destination) const {
    std::vector<Link*> possibleRoutes = getPossibleRoutes(current, destination);
    
    Link* bestLink = nullptr;
    int minDistance = INT_MAX;
    
    for (Link* link : possibleRoutes) {
        Node* next = (link->getNodeA() == current) ? link->getNodeB() : link->getNodeA();
        if (isMinimalRoute(current, next, destination)) {
            int distance = calculateManhattanDistance(next, destination);
            if (distance < minDistance) {
                minDistance = distance;
                bestLink = link;
            }
        }
    }
    
    return bestLink;
}

Link* DuatoProtocol::getDeterministicRoute(Node* current, Node* destination) const {
    int deltaX = destination->getX() - current->getX();
    int deltaY = destination->getY() - current->getY();
    
    if (deltaX != 0) {
        Direction targetDir = (deltaX > 0) ? Direction::POSITIVE_X : Direction::NEGATIVE_X;
        for (Link* link : current->getLinks()) {
            if (link->getDirection() == targetDir) {
                return link;
            }
        }
    }
    
    if (deltaY != 0) {
        Direction targetDir = (deltaY > 0) ? Direction::POSITIVE_Y : Direction::NEGATIVE_Y;
        for (Link* link : current->getLinks()) {
            if (link->getDirection() == targetDir) {
                return link;
            }
        }
    }
    
    return nullptr;
}

bool DuatoProtocol::wouldCreateDeadlock(Node* current, Link* link, VirtualChannel vc) const {
    
    if (vc == VirtualChannel::DETERMINISTIC) {
        return false;
    }
    
    (void)current; // Suppress unused parameter warning
    (void)link;    // Suppress unused parameter warning
    
    return false;
}

bool DuatoProtocol::isChannelFree(Link* link, VirtualChannel vc) const {
    return link->isVirtualChannelFree(static_cast<int>(vc));
}

std::vector<Link*> DuatoProtocol::getPossibleRoutes(Node* current, Node* destination) const {
    std::vector<Link*> routes;
    
    for (Link* link : current->getLinks()) {
        Node* next = (link->getNodeA() == current) ? link->getNodeB() : link->getNodeA();
        if (calculateManhattanDistance(next, destination) <= 
            calculateManhattanDistance(current, destination)) {
            routes.push_back(link);
        }
    }
    
    return routes;
}

int DuatoProtocol::calculateManhattanDistance(Node* from, Node* to) const {
    return abs(from->getX() - to->getX()) + abs(from->getY() - to->getY());
}

bool DuatoProtocol::isMinimalRoute(Node* current, Node* next, Node* destination) const {
    int currentDistance = calculateManhattanDistance(current, destination);
    int nextDistance = calculateManhattanDistance(next, destination);
    return nextDistance < currentDistance;
}

void DuatoProtocol::setNetwork(Network* network) {
    this->network = network;
}

Network* DuatoProtocol::getNetwork() const {
    return network;
}

void DuatoProtocol::setConfig(const Config* config) {
    this->config = config;
}