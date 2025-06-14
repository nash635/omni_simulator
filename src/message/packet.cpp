/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "message/packet.h"
#include "message/message.h"

Packet::Packet(const Message& message) 
    : sourceId(message.getSourceId()), destinationId(message.getDestinationId()), 
      injectionTime(message.getTimestamp()), hopCount(0), 
      sourceNode(nullptr), destinationNode(nullptr), currentNode(nullptr) {
}

Packet::~Packet() {
}

int Packet::getSourceId() const {
    return sourceId;
}

int Packet::getDestinationId() const {
    return destinationId;
}

int Packet::getInjectionTime() const {
    return injectionTime;
}

int Packet::getHopCount() const {
    return hopCount;
}

void Packet::setHopCount(int count) {
    hopCount = count;
}

Node* Packet::getSourceNode() const {
    return sourceNode;
}

void Packet::setSourceNode(Node* node) {
    sourceNode = node;
}

Node* Packet::getDestinationNode() const {
    return destinationNode;
}

void Packet::setDestinationNode(Node* node) {
    destinationNode = node;
}

Node* Packet::getCurrentNode() const {
    return currentNode;
}

void Packet::setCurrentNode(Node* node) {
    currentNode = node;
}

std::string Packet::toString() const {
    return "Packet[" + std::to_string(sourceId) + "->" + std::to_string(destinationId) + 
           ", hops=" + std::to_string(hopCount) + "]";
}