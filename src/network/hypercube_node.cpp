/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "network/hypercube_node.h"
#include "network/link.h"

HypercubeNode::HypercubeNode(int id, const std::vector<int>& coords, int dim) 
    : Node(id), coordinates(coords), dimension(dim) {
}

const std::vector<int>& HypercubeNode::getCoordinates() const {
    return coordinates;
}

int HypercubeNode::getDimension() const {
    return dimension;
}

HypercubeNode* HypercubeNode::getNeighborInDimension(int dim) const {
    if (dim < 0 || dim >= dimension) {
        return nullptr;
    }
    
    for (Link* link : getLinks()) {
        if (!link) continue;
        
        Node* otherNode = nullptr;
        if (link->getSourceNode() == this) {
            otherNode = link->getDestinationNode();
        } else if (link->getDestinationNode() == this) {
            otherNode = link->getSourceNode();
        }
        
        if (!otherNode) continue;
        
        HypercubeNode* otherHyperNode = dynamic_cast<HypercubeNode*>(otherNode);
        if (!otherHyperNode) continue;
        
        if (isNeighborInDimension(otherHyperNode, dim)) {
            return otherHyperNode;
        }
    }
    
    return nullptr;
}

bool HypercubeNode::isNeighborInDimension(const HypercubeNode* other, int dim) const {
    if (!other || dim < 0 || dim >= dimension) {
        return false;
    }
    
    const auto& otherCoords = other->getCoordinates();
    
    if (coordinates.size() != otherCoords.size()) {
        return false;
    }
    
    for (int i = 0; i < dimension; ++i) {
        if (i == dim) {
            if (coordinates[i] == otherCoords[i]) {
                return false;
            }
        } else {
            if (coordinates[i] != otherCoords[i]) {
                return false;
            }
        }
    }
    
    return true;
}