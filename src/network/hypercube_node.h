/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef HYPERCUBE_NODE_H
#define HYPERCUBE_NODE_H

#include "node.h"
#include <vector>

class HypercubeNode : public Node {
private:
    std::vector<int> coordinates;
    int dimension;
    
public:
    HypercubeNode(int id, const std::vector<int>& coords, int dim);
    
    const std::vector<int>& getCoordinates() const;
    int getDimension() const;
    
    HypercubeNode* getNeighborInDimension(int dim) const;
    bool isNeighborInDimension(const HypercubeNode* other, int dim) const;
};

#endif // HYPERCUBE_NODE_H