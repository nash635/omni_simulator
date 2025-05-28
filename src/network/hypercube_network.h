/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef HYPERCUBE_NETWORK_H
#define HYPERCUBE_NETWORK_H

#include "network.h"
#include <vector>

class HypercubeNode; // Forward declaration

class HypercubeNetwork : public Network {
private:
    int dimension;
    int totalNodes;
    std::vector<HypercubeNode*> hypercubeNodes;
    
public:
    HypercubeNetwork(int dimension);
    ~HypercubeNetwork();
    
    void initializeTopology() override;
    Node* getNode(int x, int y) override;
    
    int getDimension() const { return dimension; }
    int getTotalNodes() const { return totalNodes; }
    HypercubeNode* getHypercubeNode(int nodeId) const;
    
    int getNodeId(const std::vector<int>& coordinates) const;
    std::vector<int> getCoordinates(int nodeId) const;
    bool areNeighbors(int node1, int node2) const;
    int getHammingDistance(int node1, int node2) const;
    
private:
    void createHypercubeNodes();
    void createHypercubeLinks();
};

#endif // HYPERCUBE_NETWORK_H