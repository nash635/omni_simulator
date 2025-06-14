/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <vector>

class Node;
class Link;

class Network {
public:
    Network(int width, int height);
    virtual ~Network();
    
    virtual Node* getNode(int x, int y);  // Add virtual
    virtual void initializeTopology();    // Add virtual function
    std::vector<Link*> getLinks();
    
protected:  // Change to protected for subclass access
    void createNodes();
    void createLinks();
    
    int width;
    int height;
    std::vector<std::vector<Node*>> nodes;
    std::vector<Link*> links;  // Change to protected
};

#endif // NETWORK_H