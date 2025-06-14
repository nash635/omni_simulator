/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "routing/duato_hypercube_protocol.h"
#include "network/hypercube_network.h"
#include "network/hypercube_node.h"
#include "utils/config.h"

DuatoHypercubeProtocol::DuatoHypercubeProtocol(HypercubeNetwork* network) 
    : EcubeRouting(network), config(nullptr) {
}

DuatoHypercubeProtocol::DuatoHypercubeProtocol(HypercubeNetwork* network, const Config* config) 
    : EcubeRouting(network), config(config) {
}

void DuatoHypercubeProtocol::routeMessage(Message& message, Node* source, Node* destination) {
    RoutingResult result = routeMessageWithStats(message, source, destination);
}

RoutingResult DuatoHypercubeProtocol::routeMessageWithStats(Message& message, Node* source, Node* destination) {
    RoutingResult result;
    result.success = false;
    result.hopCount = 0;
    result.totalDelay = 0.0;
    
    HypercubeNode* currentNode = static_cast<HypercubeNode*>(source);
    HypercubeNode* destNode = static_cast<HypercubeNode*>(destination);
    
    if (!currentNode || !destNode) {
        return result;
    }
    
    result.path.push_back(currentNode);
    
    while (currentNode != destNode) {
        int nextDim = selectNextDimension(currentNode, destNode);
        if (nextDim == -1) break;
        
        VirtualChannel vc = selectVirtualChannel(currentNode, nextDim, destNode);
        
        HypercubeNode* nextNode = currentNode->getNeighborInDimension(nextDim);
        if (!nextNode) {
            return result;
        }
        
        if (!isVirtualChannelAvailable(currentNode, nextNode, vc)) {
            if (vc == VirtualChannel::ADAPTIVE) {
                vc = VirtualChannel::DETERMINISTIC;
                if (!isVirtualChannelAvailable(currentNode, nextNode, vc)) {
                    return result;
                }
            } else {
                return result;
            }
        }
        
        result.path.push_back(nextNode);
        currentNode = nextNode;
        result.hopCount++;
        
        if (result.hopCount > hypercubeNetwork->getDimension()) {
            return result;
        }
    }
    
    if (currentNode == destNode) {
        result.success = true;
        result.totalDelay = calculateRouteDelay(result.path);
    }
    
    return result;
}

int DuatoHypercubeProtocol::selectNextDimension(Node* current, Node* destination) const {
    HypercubeNode* currentHC = static_cast<HypercubeNode*>(current);
    HypercubeNode* destHC = static_cast<HypercubeNode*>(destination);
    
    if (!currentHC || !destHC) return -1;
    
    std::vector<int> dimensionPriorities = getDeterministicDimensionOrder(currentHC, destHC);
    
    if (canUseAdaptiveChannel(currentHC, -1, destHC)) {
        return selectAdaptiveDimension(currentHC, destHC);
    }
    
    for (int dim : dimensionPriorities) {
        if (needsRoutingInDimension(currentHC, destHC, dim)) {
            return dim;
        }
    }
    
    return -1;
}

VirtualChannel DuatoHypercubeProtocol::selectVirtualChannel(HypercubeNode* current, int dimension, HypercubeNode* destination) const {
    if (canUseAdaptiveChannel(current, dimension, destination)) {
        return VirtualChannel::ADAPTIVE;
    }
    return VirtualChannel::DETERMINISTIC;
}

bool DuatoHypercubeProtocol::canUseAdaptiveChannel(HypercubeNode* current, int targetDim, HypercubeNode* destination) const {
    
    if (targetDim >= 0) {
        if (!needsRoutingInDimension(current, destination, targetDim)) {
            return false;
        }
    }
    
    return !wouldCreateCycle(current, targetDim, VirtualChannel::ADAPTIVE);
}

bool DuatoHypercubeProtocol::mustUseDeterministicChannel(HypercubeNode* current, HypercubeNode* destination) const {
    int hammingDistance = hypercubeNetwork->getHammingDistance(current->getId(), destination->getId());
    return hammingDistance <= 1;
}

bool DuatoHypercubeProtocol::wouldCreateCycle(HypercubeNode* current, int dimension, VirtualChannel vc) const {
    if (vc == VirtualChannel::DETERMINISTIC) {
        return false;
    }
    
    return false;
}

std::vector<int> DuatoHypercubeProtocol::getDeterministicDimensionOrder(HypercubeNode* source, HypercubeNode* dest) const {
    std::vector<int> order;
    
    if (config) {
        order = config->getDimensionPriorities();
    }
    
    if (order.empty()) {
        for (int i = 0; i < hypercubeNetwork->getDimension(); ++i) {
            order.push_back(i);
        }
    }
    
    return order;
}

bool DuatoHypercubeProtocol::needsRoutingInDimension(HypercubeNode* current, HypercubeNode* destination, int dimension) const {
    const auto& currentCoords = current->getCoordinates();
    const auto& destCoords = destination->getCoordinates();
    
    if (dimension >= static_cast<int>(currentCoords.size()) || 
        dimension >= static_cast<int>(destCoords.size())) {
        return false;
    }
    
    return currentCoords[dimension] != destCoords[dimension];
}

int DuatoHypercubeProtocol::selectAdaptiveDimension(HypercubeNode* current, HypercubeNode* destination) const {
    std::vector<int> validDimensions;
    
    for (int dim = 0; dim < hypercubeNetwork->getDimension(); ++dim) {
        if (needsRoutingInDimension(current, destination, dim)) {
            validDimensions.push_back(dim);
        }
    }
    
    if (validDimensions.empty()) return -1;
    
    return validDimensions[0];
}

bool DuatoHypercubeProtocol::isVirtualChannelAvailable(HypercubeNode* from, HypercubeNode* to, VirtualChannel vc) const {
    return true;
}

std::string DuatoHypercubeProtocol::getAlgorithmDescription() const {
    return "Duato's Deadlock-Free Protocol for Hypercubes using E-cube as baseline routing";
}

std::string DuatoHypercubeProtocol::getBaselineAlgorithm() const {
    return "E-cube Routing";
}