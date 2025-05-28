/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef NODE_H
#define NODE_H

#include <vector>
#include <queue>

#include "virtual_channel.h"

// Forward declarations
class Link;
class Packet;
class Message;

class Node {
public:
    Node(int x, int y);
    Node(int id);
    virtual ~Node();
    
    void addLink(Link* link);
    const std::vector<Link*>& getLinks() const;
    int getX() const;
    int getY() const;
    int getId() const;
    
    // Virtual channel support
    void receiveMessage(Message* message, VirtualChannel vc);
    Message* sendMessage(VirtualChannel vc);
    bool hasMessage(VirtualChannel vc) const;
    bool canRoute(Node* destination, VirtualChannel vc) const;

    bool hasArrivedPacket() const;
    Packet* getArrivedPacket();
    void addArrivedPacket(Packet* packet);

private:
    int id;
    int x, y;
    std::vector<Link*> links;
    std::queue<Packet*> arrivedPackets;
    
    std::queue<Message*> adaptiveChannel;
    std::queue<Message*> deterministicChannel;
};

#endif // NODE_H