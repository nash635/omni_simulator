/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "routing/ecube_routing.h"
#include "network/hypercube_network.h"
#include "network/hypercube_node.h"
#include "message/message.h"

EcubeRouting::EcubeRouting(HypercubeNetwork* network) : hypercubeNetwork(network) {
    this->network = network;
}

EcubeRouting::~EcubeRouting() {
}

void EcubeRouting::routeMessage(Message& message, Node* source, Node* destination) {
    if (!source || !destination) {
        return;
    }
    
    HypercubeNode* currentNode = dynamic_cast<HypercubeNode*>(source);
    HypercubeNode* destNode = dynamic_cast<HypercubeNode*>(destination);
    
    if (!currentNode || !destNode) {
        return;
    }
    
    message.setCurrentNode(source);
    message.setDestinationNode(destination);
}

RoutingResult EcubeRouting::routeMessageWithStats(Message& message, Node* source, Node* destination) {
    RoutingResult result;
    
    routeMessage(message, source, destination);
    
    result.path = calculatePath(source, destination);
    result.success = !result.path.empty();
    result.hopCount = static_cast<int>(result.path.size()) - 1;
    result.delay = result.hopCount;
    result.totalDelay = calculateRouteDelay(result.path);
    
    return result;
}

void EcubeRouting::setNetwork(Network* network) {
    this->network = network;
    this->hypercubeNetwork = dynamic_cast<HypercubeNetwork*>(network);
}

Network* EcubeRouting::getNetwork() const {
    return hypercubeNetwork;
}

std::vector<Node*> EcubeRouting::calculatePath(Node* source, Node* destination) {
    std::vector<Node*> path;
    
    if (!source || !destination) {
        return path;
    }
    
    HypercubeNode* hypercubeSource = dynamic_cast<HypercubeNode*>(source);
    HypercubeNode* hypercubeDest = dynamic_cast<HypercubeNode*>(destination);
    
    if (hypercubeSource && hypercubeDest) {
        path.push_back(source);
        
        auto sourceCoords = hypercubeSource->getCoordinates();
        auto destCoords = hypercubeDest->getCoordinates();
        
        Node* current = source;
        while (current != destination) {
            HypercubeNode* currentHyper = dynamic_cast<HypercubeNode*>(current);
            if (!currentHyper) break;
            
            auto currentCoords = currentHyper->getCoordinates();
            
            Node* next = nullptr;
            for (int dim = 0; dim < static_cast<int>(currentCoords.size()); ++dim) {
                if (currentCoords[dim] != destCoords[dim]) {
                    next = currentHyper->getNeighborInDimension(dim);
                    break;
                }
            }
            
            if (!next || next == current) break;
            
            path.push_back(next);
            current = next;
        }
    }
    
    return path;
}

int EcubeRouting::selectNextDimension(Node* current, Node* destination) const {
    HypercubeNode* currentHC = dynamic_cast<HypercubeNode*>(current);
    HypercubeNode* destHC = dynamic_cast<HypercubeNode*>(destination);
    
    if (!currentHC || !destHC) return -1;
    
    std::vector<int> differences = calculateDimensionDifferences(currentHC, destHC);
    
    for (int dim = 0; dim < static_cast<int>(differences.size()); ++dim) {
        if (differences[dim] != 0) {
            return dim;
        }
    }
    
    return -1;
}

std::vector<int> EcubeRouting::calculateDimensionDifferences(Node* source, Node* destination) const {
    HypercubeNode* sourceHC = dynamic_cast<HypercubeNode*>(source);
    HypercubeNode* destHC = dynamic_cast<HypercubeNode*>(destination);
    
    std::vector<int> differences;
    if (!sourceHC || !destHC) return differences;
    
    const auto& sourceCoords = sourceHC->getCoordinates();
    const auto& destCoords = destHC->getCoordinates();
    
    for (size_t i = 0; i < sourceCoords.size() && i < destCoords.size(); ++i) {
        differences.push_back(destCoords[i] - sourceCoords[i]);
    }
    
    return differences;
}

bool EcubeRouting::isDestinationReached(Node* current, Node* destination) const {
    return current == destination;
}

double EcubeRouting::calculateRouteDelay(const std::vector<Node*>& path) const {
    if (path.size() < 2) return 0.0;
    
    double linkLatency = 1.0;
    double totalDelay = (path.size() - 1) * linkLatency;
    
    totalDelay += (path.size() - 1) * 0.5;
    
    return totalDelay;
}