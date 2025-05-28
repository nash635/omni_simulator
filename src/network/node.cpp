/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "network/node.h"
#include "network/link.h"
#include "message/message.h"
#include "message/packet.h"
#include <queue>

Node::Node(int id) : id(id), x(0), y(0) {
}

Node::Node(int x, int y) : id(x * 100 + y), x(x), y(y) {
}

Node::~Node() {
    while (!arrivedPackets.empty()) {
        delete arrivedPackets.front();
        arrivedPackets.pop();
    }
    
    while (!adaptiveChannel.empty()) {
        delete adaptiveChannel.front();
        adaptiveChannel.pop();
    }
    
    while (!deterministicChannel.empty()) {
        delete deterministicChannel.front();
        deterministicChannel.pop();
    }
}

void Node::addLink(Link* link) {
    links.push_back(link);
}

const std::vector<Link*>& Node::getLinks() const {
    return links;
}

int Node::getX() const {
    return x;
}

int Node::getY() const {
    return y;
}

int Node::getId() const {
    return id;
}

void Node::receiveMessage(Message* message, VirtualChannel vc) {
    if (vc == VirtualChannel::ADAPTIVE) {
        adaptiveChannel.push(message);
    } else {
        deterministicChannel.push(message);
    }
}

Message* Node::sendMessage(VirtualChannel vc) {
    Message* message = nullptr;
    
    if (vc == VirtualChannel::ADAPTIVE && !adaptiveChannel.empty()) {
        message = adaptiveChannel.front();
        adaptiveChannel.pop();
    } else if (vc == VirtualChannel::DETERMINISTIC && !deterministicChannel.empty()) {
        message = deterministicChannel.front();
        deterministicChannel.pop();
    }
    
    return message;
}

bool Node::hasMessage(VirtualChannel vc) const {
    if (vc == VirtualChannel::ADAPTIVE) {
        return !adaptiveChannel.empty();
    } else {
        return !deterministicChannel.empty();
    }
}

bool Node::canRoute(Node* destination, VirtualChannel vc) const {
    for (Link* link : links) {
        if (link && link->canTransmit(vc)) {
            return true;
        }
    }
    return false;
}

bool Node::hasArrivedPacket() const {
    return !arrivedPackets.empty();
}

Packet* Node::getArrivedPacket() {
    if (arrivedPackets.empty()) {
        return nullptr;
    }
    
    Packet* packet = arrivedPackets.front();
    arrivedPackets.pop();
    return packet;
}

void Node::addArrivedPacket(Packet* packet) {
    arrivedPackets.push(packet);
}