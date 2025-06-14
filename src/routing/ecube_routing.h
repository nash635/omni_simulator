/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef ECUBE_ROUTING_H
#define ECUBE_ROUTING_H

#include "routing_algorithm.h"
#include <vector>

class HypercubeNetwork;
class HypercubeNode;

class EcubeRouting : public RoutingAlgorithm {
public:
    EcubeRouting(HypercubeNetwork* network);
    ~EcubeRouting();
    
    void routeMessage(Message& message, Node* source, Node* destination) override;
    RoutingResult routeMessageWithStats(Message& message, Node* source, Node* destination) override;
    
    void setNetwork(Network* network) override;
    Network* getNetwork() const override;
    
protected:
    HypercubeNetwork* hypercubeNetwork;
    std::vector<Node*> calculatePath(Node* source, Node* destination);
    
    virtual int selectNextDimension(Node* current, Node* destination) const;
    std::vector<int> calculateDimensionDifferences(Node* source, Node* destination) const;
    bool isDestinationReached(Node* current, Node* destination) const;
    double calculateRouteDelay(const std::vector<Node*>& path) const;
};

#endif // ECUBE_ROUTING_H