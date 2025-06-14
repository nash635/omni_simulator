/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "network/network.h"
#include "routing/routing_algorithm.h"
#include "traffic/traffic_pattern.h"
#include "metrics/metrics.h"
#include <vector>
#include <map>
#include <queue>

// Forward declarations
class Config;
class Packet;
class Node;
class HypercubeNetwork;

class Simulator {
public:
    Simulator(int networkSizeX, int networkSizeY);
    Simulator(HypercubeNetwork* hypercubeNetwork);
    ~Simulator();
    
    void initializeNetwork();
    void runSimulation(double injectionRate, const Config& config);
    void collectMetrics();
    Metrics* getMetrics() const;
    void reset();

    void setNetwork(Network* network);
    void setRoutingAlgorithm(RoutingAlgorithm* algorithm);

private:
    int injectPackets(double injectionRate);
    void routePackets();
    int receivePackets();
    void updateCurrentCycle(int cycle);
    
    double getNodeBufferUtilization(Node* node);
    int calculateCongestionDelay(Node* source, Node* destination);
    double calculateNetworkUtilization();
    void updateNetworkState();
    
    Node* calculateNextHop(Node* current, Node* destination);
    Node* calculateNextHopHypercube(Node* current, Node* destination);
    int countReceivedFlits();
    double calculateQueuingDelay(Packet* packet);
    double calculateBufferDelay(Packet* packet);

    int networkSizeX;
    int networkSizeY;
    int currentCycle;
    int maxBufferSize;
    double linkCapacity;
    bool isHypercubeMode;      
    HypercubeNetwork* hypercubeNet;
    
    Network* network;
    RoutingAlgorithm* routingAlgorithm;
    TrafficPattern* trafficPattern;
    Metrics* metrics;
    
    std::map<Node*, std::queue<Packet*>> nodeBuffers;
    std::map<std::pair<Node*, Node*>, int> linkUtilization;
    
    struct InTransitPacket {
        Packet* packetPtr;
        int arrivalCycle;
        Node* destinationNode;
        
        InTransitPacket(Packet* packet, int cycle, Node* dest) 
            : packetPtr(packet), arrivalCycle(cycle), destinationNode(dest) {}
    };
    
    std::vector<InTransitPacket> inTransitPackets;
    double currentInjectionRate;
};

#endif // SIMULATOR_H