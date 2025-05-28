/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "network/hypercube_network.h"
#include "network/hypercube_node.h"
#include "network/link.h"  // Ensure this header is included to use Direction enum
#include <cmath>

HypercubeNetwork::HypercubeNetwork(int dimension) : Network(0, 0), dimension(dimension) {
    totalNodes = static_cast<int>(std::pow(2, dimension));
}

void HypercubeNetwork::initializeTopology() {
    createHypercubeNodes();
    createHypercubeLinks();
}

void HypercubeNetwork::createHypercubeNodes() {
    hypercubeNodes.clear();
    hypercubeNodes.reserve(totalNodes);
    
    for (int nodeId = 0; nodeId < totalNodes; ++nodeId) {
        std::vector<int> coordinates = getCoordinates(nodeId);
        HypercubeNode* node = new HypercubeNode(nodeId, coordinates, dimension);
        hypercubeNodes.push_back(node);
    }
}

void HypercubeNetwork::createHypercubeLinks() {
    for (int nodeId = 0; nodeId < totalNodes; ++nodeId) {
        HypercubeNode* currentNode = hypercubeNodes[nodeId];
        
        for (int dim = 0; dim < dimension; ++dim) {
            int neighborId = nodeId ^ (1 << dim); // Flip the dim-th bit
            if (neighborId > nodeId) { // Avoid duplicate link creation
                HypercubeNode* neighborNode = hypercubeNodes[neighborId];
                
                Direction linkDirection;
                switch (dim % 4) {
                    case 0: linkDirection = Direction::POSITIVE_X; break;
                    case 1: linkDirection = Direction::POSITIVE_Y; break;
                    case 2: linkDirection = Direction::NEGATIVE_X; break;
                    case 3: linkDirection = Direction::NEGATIVE_Y; break;
                    default: linkDirection = Direction::POSITIVE_X; break;
                }
                
                Link* link = new Link(currentNode, neighborNode, linkDirection);
                links.push_back(link);
                currentNode->addLink(link);
                neighborNode->addLink(link);
            }
        }
    }
}

std::vector<int> HypercubeNetwork::getCoordinates(int nodeId) const {
    std::vector<int> coordinates(dimension);
    for (int i = 0; i < dimension; ++i) {
        coordinates[i] = (nodeId >> i) & 1;
    }
    return coordinates;
}

int HypercubeNetwork::getNodeId(const std::vector<int>& coordinates) const {
    int nodeId = 0;
    for (int i = 0; i < dimension; ++i) {
        if (coordinates[i]) {
            nodeId |= (1 << i);
        }
    }
    return nodeId;
}

HypercubeNode* HypercubeNetwork::getHypercubeNode(int nodeId) const {
    if (nodeId >= 0 && nodeId < totalNodes) {
        return hypercubeNodes[nodeId];
    }
    return nullptr;
}

bool HypercubeNetwork::areNeighbors(int node1, int node2) const {
    int xorResult = node1 ^ node2;
    return xorResult && !(xorResult & (xorResult - 1));
}

int HypercubeNetwork::getHammingDistance(int node1, int node2) const {
    int xorResult = node1 ^ node2;
    int distance = 0;
    while (xorResult) {
        distance += xorResult & 1;
        xorResult >>= 1;
    }
    return distance;
}

Node* HypercubeNetwork::getNode(int x, int y) {
    // Map 2D coordinates to hypercube node ID
    if (dimension >= 2) {
        int nodeId = x + y * (1 << (dimension/2));
        if (nodeId < totalNodes) {
            return hypercubeNodes[nodeId];
        }
    }
    return nullptr;
}

HypercubeNetwork::~HypercubeNetwork() {
    for (HypercubeNode* node : hypercubeNodes) {
        delete node;
    }
    hypercubeNodes.clear();
}