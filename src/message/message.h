/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef MESSAGE_H
#define MESSAGE_H

// Forward declarations to avoid including network/node.h
class Node;

class Message {
public:
    Message(int sourceId, int destinationId, int timestamp);
    ~Message();
    
    int getSourceId() const;
    int getDestinationId() const;
    int getTimestamp() const;
    
    void setCurrentNode(Node* node);
    void setDestinationNode(Node* node);
    Node* getCurrentNode() const;
    Node* getDestinationNode() const;

private:
    int sourceId;
    int destinationId;
    int timestamp;
    Node* currentNode;
    Node* destinationNode;
};

#endif // MESSAGE_H