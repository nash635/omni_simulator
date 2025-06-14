/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef DUATO_HYPERCUBE_PROTOCOL_H
#define DUATO_HYPERCUBE_PROTOCOL_H

#include "ecube_routing.h"
#include "network/virtual_channel.h"
#include <vector>
#include <string>  // Add this header file

class HypercubeNetwork;
class HypercubeNode;
class Config;

class DuatoHypercubeProtocol : public EcubeRouting {
public:
    DuatoHypercubeProtocol(HypercubeNetwork* network);
    DuatoHypercubeProtocol(HypercubeNetwork* network, const Config* config);
    
    void routeMessage(Message& message, Node* source, Node* destination) override;
    RoutingResult routeMessageWithStats(Message& message, Node* source, Node* destination) override;
    
    std::string getAlgorithmDescription() const;
    std::string getBaselineAlgorithm() const;

private:
    const Config* config;
    
    bool canUseAdaptiveChannel(HypercubeNode* current, int targetDim, HypercubeNode* destination) const;
    bool mustUseDeterministicChannel(HypercubeNode* current, HypercubeNode* destination) const;
    
    VirtualChannel selectVirtualChannel(HypercubeNode* current, int dimension, HypercubeNode* destination) const;
    bool wouldCreateCycle(HypercubeNode* current, int dimension, VirtualChannel vc) const;
    
    int selectNextDimension(Node* current, Node* destination) const override;
    
    std::vector<int> getDeterministicDimensionOrder(HypercubeNode* source, HypercubeNode* dest) const;
    bool needsRoutingInDimension(HypercubeNode* current, HypercubeNode* destination, int dimension) const;
    int selectAdaptiveDimension(HypercubeNode* current, HypercubeNode* destination) const;
    bool isVirtualChannelAvailable(HypercubeNode* from, HypercubeNode* to, VirtualChannel vc) const;
};

#endif // DUATO_HYPERCUBE_PROTOCOL_H