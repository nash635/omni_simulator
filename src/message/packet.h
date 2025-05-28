/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef PACKET_H
#define PACKET_H

#include <string>

// Forward declarations to avoid circular dependencies
class Message;
class Node;

class Packet {
public:
    Packet(const Message& message);
    ~Packet();
    
    // Basic information retrieval
    int getSourceId() const;
    int getDestinationId() const;
    int getInjectionTime() const;
    
    // Hop count management
    int getHopCount() const;
    void setHopCount(int count);
    
    // Node management
    Node* getSourceNode() const;
    void setSourceNode(Node* node);
    Node* getDestinationNode() const;
    void setDestinationNode(Node* node);
    Node* getCurrentNode() const;
    void setCurrentNode(Node* node);
    
    // Debug and logging methods
    std::string toString() const;

private:
    int sourceId;
    int destinationId;
    int injectionTime;
    int hopCount;
    
    // Node pointers (not owned, just references)
    Node* sourceNode;
    Node* destinationNode;
    Node* currentNode;
};

#endif // PACKET_H