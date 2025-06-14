/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef ROUTING_ALGORITHM_H
#define ROUTING_ALGORITHM_H

#include <vector>

// Forward declarations
class Node;
class Message;
class Network;

// Routing result structure
struct RoutingResult {
    bool success;
    std::vector<Node*> path;
    int hopCount;
    double delay;
    double totalDelay;
    
    RoutingResult() : success(false), hopCount(0), delay(0.0), totalDelay(0.0) {}
};

class RoutingAlgorithm {
public:
    RoutingAlgorithm() : network(nullptr) {}
    virtual ~RoutingAlgorithm() {}
    
    virtual void routeMessage(Message& message, Node* source, Node* destination) = 0;
    virtual RoutingResult routeMessageWithStats(Message& message, Node* source, Node* destination) = 0;
    
    virtual void setNetwork(Network* net) { network = net; }
    virtual Network* getNetwork() const { return network; }
    
protected:
    Network* network;
};

#endif // ROUTING_ALGORITHM_H