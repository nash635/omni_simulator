/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "network/link.h"
#include "network/node.h"

Link::Link(Node* source, Node* destination, Direction direction)
    : sourceNode(source), destinationNode(destination), direction(direction),
      adaptiveChannelOccupied(false), deterministicChannelOccupied(false) {
}

Link::~Link() {
}

Node* Link::getSourceNode() const {
    return sourceNode;
}

Node* Link::getDestinationNode() const {
    return destinationNode;
}

Direction Link::getDirection() const {
    return direction;
}

Node* Link::getNodeA() const {
    return sourceNode;
}

Node* Link::getNodeB() const {
    return destinationNode;
}

bool Link::canTransmit(VirtualChannel vc) const {
    switch (vc) {
        case VirtualChannel::ADAPTIVE:
            return !adaptiveChannelOccupied;
        case VirtualChannel::DETERMINISTIC:
            return !deterministicChannelOccupied;
        default:
            return false;
    }
}

void Link::transmit(VirtualChannel vc) {
    switch (vc) {
        case VirtualChannel::ADAPTIVE:
            adaptiveChannelOccupied = true;
            break;
        case VirtualChannel::DETERMINISTIC:
            deterministicChannelOccupied = true;
            break;
        default:
            break;
    }
}

bool Link::isOccupied(VirtualChannel vc) const {
    return !canTransmit(vc);
}

bool Link::isVirtualChannelFree(int vc) const {
    if (vc == static_cast<int>(VirtualChannel::ADAPTIVE)) {
        return canTransmit(VirtualChannel::ADAPTIVE);
    } else if (vc == static_cast<int>(VirtualChannel::DETERMINISTIC)) {
        return canTransmit(VirtualChannel::DETERMINISTIC);
    }
    return false;
}