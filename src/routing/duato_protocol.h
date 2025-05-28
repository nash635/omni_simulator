/*
 * omni_simulator - Network Routing Simulator
 * Duato's Deadlock-Free Routing Protocol Implementation
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef DUATO_PROTOCOL_H
#define DUATO_PROTOCOL_H

#include "routing_algorithm.h"
#include "network/node.h"
#include "network/link.h"
#include "network/virtual_channel.h"
#include "message/message.h"
#include <vector>
#include <set>
#include <string>

class Network;
class Config; // Forward declaration

class DuatoProtocol : public RoutingAlgorithm {
public:
    DuatoProtocol();
    DuatoProtocol(Network* network);
    DuatoProtocol(Network* network, const Config* config);
    ~DuatoProtocol();

    void routeMessage(Message& message, Node* source, Node* destination) override;
    RoutingResult routeMessageWithStats(Message& message, Node* source, Node* destination) override;
    
    void setNetwork(Network* network) override;
    Network* getNetwork() const override;
    
    void setConfig(const Config* config);

private:
    Network* network;
    const Config* config;
    
    double calculatePathDelay(const std::vector<Node*>& path) const;
    std::vector<Node*> findPath(Node* source, Node* destination) const;
    
    bool canUseAdaptiveChannel(Node* current, Node* next, Node* destination) const;
    bool mustUseDeterministicChannel(Node* current, Node* destination) const;
    Link* selectAdaptiveRoute(Node* current, Node* destination) const;
    Link* getDeterministicRoute(Node* current, Node* destination) const;
    
    bool wouldCreateDeadlock(Node* current, Link* link, VirtualChannel vc) const;
    bool isChannelFree(Link* link, VirtualChannel vc) const;
    
    std::vector<Link*> getPossibleRoutes(Node* current, Node* destination) const;
    int calculateManhattanDistance(Node* from, Node* to) const;
    bool isMinimalRoute(Node* current, Node* next, Node* destination) const;
    
    void debugLog(const std::string& message) const;
    void debugRouting(const std::string& action, Node* current, Node* next, const std::string& channel) const;
    void debugDeadlock(Node* current, const std::string& reason) const;
};

#endif // DUATO_PROTOCOL_H