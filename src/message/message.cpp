/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "message/message.h"

Message::Message(int sourceId, int destinationId, int timestamp)
    : sourceId(sourceId), destinationId(destinationId), timestamp(timestamp),
      currentNode(nullptr), destinationNode(nullptr) {
}

Message::~Message() {
}

// Implementation of routing-related methods
int Message::getSourceId() const {
    return sourceId;
}

int Message::getDestinationId() const {
    return destinationId;
}

int Message::getTimestamp() const {
    return timestamp;
}

void Message::setCurrentNode(Node* node) {
    currentNode = node;
}

void Message::setDestinationNode(Node* node) {
    destinationNode = node;
}

Node* Message::getCurrentNode() const {
    return currentNode;
}

Node* Message::getDestinationNode() const {
    return destinationNode;
}