/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef LINK_H
#define LINK_H

#include "virtual_channel.h"

class Node;

enum class Direction {
    POSITIVE_X,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z
};

class Link {
public:
    Link(Node* source, Node* destination, Direction direction);
    ~Link();
    
    Node* getSourceNode() const;
    Node* getDestinationNode() const;
    Direction getDirection() const;
    
    Node* getNodeA() const;
    Node* getNodeB() const;
    
    bool canTransmit(VirtualChannel vc) const;
    void transmit(VirtualChannel vc);
    bool isOccupied(VirtualChannel vc) const;
    
    bool isVirtualChannelFree(int vc) const;

private:
    Node* sourceNode;
    Node* destinationNode;
    Direction direction;
    
    bool adaptiveChannelOccupied;
    bool deterministicChannelOccupied;
};

#endif // LINK_H