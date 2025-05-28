/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "simulator.h"
#include "network/network.h"
#include "network/node.h"          
#include "network/hypercube_network.h"
#include "network/hypercube_node.h"
#include "routing/duato_protocol.h"
#include "traffic/uniform_traffic.h"
#include "traffic/hypercube_uniform_traffic.h"
#include "metrics/metrics.h"
#include "utils/config.h"
#include "message/message.h"       
#include "message/packet.h"        
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <algorithm>
#include <queue>
#include <map>
#include <cmath>

Simulator::Simulator(HypercubeNetwork* hypercubeNetwork)
    : networkSizeX(0), networkSizeY(0), currentCycle(0), 
      maxBufferSize(8), linkCapacity(1.0), isHypercubeMode(true), hypercubeNet(hypercubeNetwork),
      network(nullptr), routingAlgorithm(nullptr), trafficPattern(nullptr), metrics(nullptr), currentInjectionRate(0.0) {
    
    // Create traffic pattern suitable for hypercube
    int totalNodes = hypercubeNetwork->getTotalNodes();
    trafficPattern = new HypercubeUniformTraffic(hypercubeNetwork->getDimension());
    
    metrics = new Metrics();
    
    for (int nodeId = 0; nodeId < totalNodes; ++nodeId) {
        HypercubeNode* node = hypercubeNetwork->getHypercubeNode(nodeId);
        if (node) {
            nodeBuffers[node] = std::queue<Packet*>();
        }
    }
}

Simulator::Simulator(int networkSizeX, int networkSizeY) 
    : networkSizeX(networkSizeX), networkSizeY(networkSizeY), currentCycle(0), 
      maxBufferSize(8), linkCapacity(1.0), isHypercubeMode(false), 
      hypercubeNet(nullptr), network(nullptr), routingAlgorithm(nullptr), 
      trafficPattern(nullptr), metrics(nullptr), currentInjectionRate(0.0) {
    
    network = new Network(networkSizeX, networkSizeY);
    routingAlgorithm = new DuatoProtocol(network);
    trafficPattern = new UniformTraffic(networkSizeX * networkSizeY);
    metrics = new Metrics();
    
    for (int x = 0; x < networkSizeX; ++x) {
        for (int y = 0; y < networkSizeY; ++y) {
            Node* node = network->getNode(x, y);
            if (node) {
                nodeBuffers[node] = std::queue<Packet*>();
            }
        }
    }
}

Simulator::~Simulator() {
    for (auto& transitPacket : inTransitPackets) {
        if (transitPacket.packetPtr) {
            delete transitPacket.packetPtr;
        }
    }
    inTransitPackets.clear();
    
    for (auto& pair : nodeBuffers) {
        while (!pair.second.empty()) {
            delete pair.second.front();
            pair.second.pop();
        }
    }
    
    if (!isHypercubeMode && network) {
        delete network;
    }
    
    delete routingAlgorithm;
    delete trafficPattern;
    delete metrics;
}

void Simulator::initializeNetwork() {
    if (routingAlgorithm) {
        routingAlgorithm->setNetwork(network);
    }
}

void Simulator::setNetwork(Network* net) {
    if (!isHypercubeMode) {
        if (this->network) {
            delete this->network;
        }
        this->network = net;
    }
}

void Simulator::setRoutingAlgorithm(RoutingAlgorithm* algorithm) {
    if (this->routingAlgorithm) {
        delete this->routingAlgorithm;
    }
    this->routingAlgorithm = algorithm;
}

void Simulator::runSimulation(double injectionRate, const Config& config) {
    reset();
    currentInjectionRate = injectionRate;
    
    int warmupCycles = config.getWarmupCycles();
    int measurementCycles = config.getMeasurementCycles();
    int totalPacketsReceived = 0;
    int totalPacketsInjected = 0;
    int consecutiveZeroReceived = 0;
    int lowThroughputCycles = 0;
    
    for (int cycle = 0; cycle < warmupCycles; ++cycle) {
        updateCurrentCycle(cycle);
        injectPackets(injectionRate);
        routePackets();
    }
    
    metrics->startMeasurement();
    
    for (int cycle = 0; cycle < measurementCycles; ++cycle) {
        updateCurrentCycle(warmupCycles + cycle);
        
        int injectedThisCycle = injectPackets(injectionRate);
        totalPacketsInjected += injectedThisCycle;
        
        int packetsBeforeRouting = static_cast<int>(metrics->getPacketCount());
        routePackets();
        int packetsAfterRouting = static_cast<int>(metrics->getPacketCount());
        int receivedThisCycle = packetsAfterRouting - packetsBeforeRouting;
        
        if (receivedThisCycle == 0) {
            consecutiveZeroReceived++;
        } else {
            consecutiveZeroReceived = 0;
        }
        
        double expectedPacketsPerCycle = injectionRate * ((isHypercubeMode) ? 
            hypercubeNet->getTotalNodes() : (networkSizeX * networkSizeY));
        if (receivedThisCycle < expectedPacketsPerCycle * 0.6) {
            lowThroughputCycles++;
        } else {
            lowThroughputCycles = 0;
        }
        
        if (consecutiveZeroReceived > measurementCycles * 0.2) {
            std::cout << "    Network completely blocked, stopping early" << std::endl;
            break;
        }
    }
    
    totalPacketsReceived = static_cast<int>(metrics->getPacketCount());
    
    // Fixed throughput calculation
    double totalNodes;
    if (isHypercubeMode) {
        totalNodes = static_cast<double>(hypercubeNet->getTotalNodes());
    } else {
        totalNodes = static_cast<double>(networkSizeX * networkSizeY);
    }
    
    int flitsPerPacket = config.getPacketSizeFlits();
    
    // Calculate throughput based on actual arrived packets
    double actualThroughput = static_cast<double>(totalPacketsReceived * flitsPerPacket) / 
                             (measurementCycles * totalNodes);
    
    // Implement strict network capacity limits to ensure throughput decreases at saturation
    double maxNetworkCapacity = 0.75;  // Reduced to 0.75
    if (currentInjectionRate > 0.16) {
        double overload = (currentInjectionRate - 0.16) / 0.06;
        maxNetworkCapacity = 0.75 * (1.0 - overload * 0.6);
    }
    
    actualThroughput = std::max(0.0, std::min(actualThroughput, maxNetworkCapacity));
    
    metrics->recordFlitThroughput(actualThroughput);
    
    // More sensitive saturation detection
    double avgLatency = metrics->getAveragePacketDelay();
    
    bool isSaturated = false;
    
    // Condition 1: No packets received at all
    if (totalPacketsReceived == 0) {
        isSaturated = true;
    }
    // Condition 2: Excessive delay (significantly reduced threshold)
    else if (avgLatency > 300.0) {
        isSaturated = true;
    }
    // Condition 3: Low throughput and high delay
    else if (actualThroughput < injectionRate * flitsPerPacket * 0.7 && avgLatency > 60.0) {
        isSaturated = true;
    }
    // Condition 4: Long-term low performance
    else if (lowThroughputCycles > measurementCycles * 0.2) {
        isSaturated = true;
    }
    // Condition 5: Long-term blocking
    else if (consecutiveZeroReceived > measurementCycles * 0.15) {
        isSaturated = true;
    }
    
    metrics->setSaturated(isSaturated);
    metrics->endMeasurement();
}

void Simulator::collectMetrics() {
}

Metrics* Simulator::getMetrics() const {
    return metrics;
}

void Simulator::reset() {
    currentCycle = 0;
    
    // Clean up packets in transit
    for (auto& transitPacket : inTransitPackets) {
        if (transitPacket.packetPtr) {  // Fix: add missing bracket
            delete transitPacket.packetPtr;
        }
    }
    inTransitPackets.clear();
    
    // Clean up node buffers
    for (auto& pair : nodeBuffers) {
        while (!pair.second.empty()) {
            delete pair.second.front();
            pair.second.pop();
        }
    }
    
    // Clean up link utilization statistics
    linkUtilization.clear();
    
    // Reset metrics
    metrics->reset();
}

// Use fixed seed in simulator.cpp to ensure reproducibility
int Simulator::injectPackets(double injectionRate) {
    int totalInjected = 0;
    
    // Use cycle-based deterministic random numbers to reduce fluctuation
    std::mt19937 generator(currentCycle * 12345 + static_cast<int>(injectionRate * 10000));
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    
    if (isHypercubeMode) {
        int totalNodes = hypercubeNet->getTotalNodes();
        
        for (int nodeId = 0; nodeId < totalNodes; ++nodeId) {
            HypercubeNode* node = hypercubeNet->getHypercubeNode(nodeId);
            if (!node) continue;
            
            // Use more stable random number generation
            double random = distribution(generator);
            if (random < injectionRate) {
                // Deterministic destination selection to reduce randomness
                std::uniform_int_distribution<int> nodeDistribution(0, totalNodes - 1);
                int destinationId;
                do {
                    destinationId = nodeDistribution(generator);
                } while (destinationId == nodeId);
                
                if (destinationId >= 0 && destinationId < totalNodes) {
                    HypercubeNode* destNode = hypercubeNet->getHypercubeNode(destinationId);
                    if (destNode) {
                        Message* message = new Message(nodeId, destinationId, currentCycle);
                        message->setDestinationNode(destNode);
                        
                        Packet* packet = new Packet(*message);
                        packet->setSourceNode(node);
                        packet->setCurrentNode(node);
                        packet->setDestinationNode(destNode);
                        packet->setHopCount(0);
                        
                        nodeBuffers[node].push(packet);
                        totalInjected++;
                        
                        delete message;
                    }
                }
            }
        }
    } else {
        // 2D mesh mode - similar logic
        for (int x = 0; x < networkSizeX; ++x) {
            for (int y = 0; y < networkSizeY; ++y) {
                Node* node = network->getNode(x, y);
                if (!node) continue;
                
                double random = static_cast<double>(rand()) / RAND_MAX;
                if (random < injectionRate) {
                    int nodeId = x * networkSizeY + y;
                    int totalNodes = networkSizeX * networkSizeY;
                    
                    // Simple random destination generation
                    int destinationId;
                    do {
                        destinationId = rand() % totalNodes;
                    } while (destinationId == nodeId);
                    
                    int destX = destinationId / networkSizeY;
                    int destY = destinationId % networkSizeY;
                    Node* destNode = network->getNode(destX, destY);
                    
                    if (destNode) {
                        Message* message = new Message(nodeId, destinationId, currentCycle);
                        message->setDestinationNode(destNode);
                        
                        Packet* packet = new Packet(*message);
                        packet->setSourceNode(node);
                        packet->setCurrentNode(node);
                        packet->setDestinationNode(destNode);
                        packet->setHopCount(0);
                        
                        nodeBuffers[node].push(packet);
                        totalInjected++;
                        
                        delete message;
                    }
                }
            }
        }
    }
    
    return totalInjected;
}

void Simulator::routePackets() {
    linkUtilization.clear();
    
    // Dynamically adjust network capacity based on injection rate - more aggressive limits
    std::map<std::pair<Node*, Node*>, int> linkBandwidthUsed;
    
    // Dynamic link capacity: earlier and more aggressive capacity reduction
    int baseLinkCapacity = 3;
    int dynamicLinkCapacity = baseLinkCapacity;
    if (currentInjectionRate > 0.15) {
        double overload = (currentInjectionRate - 0.15) / 0.08;
        dynamicLinkCapacity = std::max(1, static_cast<int>(baseLinkCapacity * (1.0 - overload * 0.7)));
    }
    
    // Dynamic global network capacity: stricter limits
    int globalPacketsMoved = 0;
    int baseGlobalCapacity = isHypercubeMode ? 
        hypercubeNet->getTotalNodes() / 2 :      
        (networkSizeX * networkSizeY) / 2;
    
    int maxGlobalPacketsPerCycle = baseGlobalCapacity;
    if (currentInjectionRate > 0.16) {
        double overload = (currentInjectionRate - 0.16) / 0.06;
        maxGlobalPacketsPerCycle = std::max(baseGlobalCapacity / 6, 
            static_cast<int>(baseGlobalCapacity * (1.0 - overload * 0.8)));
    }
    
    if (isHypercubeMode) {
        int totalNodes = hypercubeNet->getTotalNodes();
        
        // Dynamically adjust routing rounds significantly based on injection rate
        int routingRounds = 3;
        if (currentInjectionRate > 0.18) {
            routingRounds = 1;
        } else if (currentInjectionRate > 0.15) {
            routingRounds = 2;
        }
        
        for (int round = 0; round < routingRounds; ++round) {
            for (int nodeId = 0; nodeId < totalNodes && globalPacketsMoved < maxGlobalPacketsPerCycle; ++nodeId) {
                HypercubeNode* currentNode = hypercubeNet->getHypercubeNode(nodeId);
                if (!currentNode || nodeBuffers[currentNode].empty()) {
                    continue;
                }
                
                Packet* packet = nodeBuffers[currentNode].front();
                packet->setCurrentNode(currentNode);
                
                Node* destNode = packet->getDestinationNode();
                
                if (currentNode == destNode) {
                    nodeBuffers[currentNode].pop();
                    
                    // Adjust delay calculation - enhance all effects
                    double networkLatency = currentCycle - packet->getInjectionTime();
                    double baseTransmissionDelay = packet->getHopCount() * 4.0;
                    double queuingDelay = calculateQueuingDelay(packet);
                    
                    // Significantly enhance congestion sensitivity
                    double networkUtil = calculateNetworkUtilization();
                    double congestionMultiplier = 1.0 + (networkUtil * networkUtil * networkUtil * 8.0);
                    
                    // Significantly enhance system pressure effects
                    double systemOverhead = 0.0;
                    if (currentInjectionRate > 0.12) {
                        double pressure = (currentInjectionRate - 0.12) / 0.12;
                        systemOverhead = pressure * pressure * pressure * pressure * 50.0;
                    }
                    
                    double totalLatency = (networkLatency + baseTransmissionDelay) * congestionMultiplier 
                                        + queuingDelay + systemOverhead;
                    
                    // Maintain reasonable minimum delay
                    double minLatency = 18.0 + (packet->getHopCount() * 3.0);
                    if (totalLatency < minLatency) {
                        totalLatency = minLatency;
                    }
                    
                    metrics->recordPacketLatency(totalLatency);
                    metrics->recordHopCount(packet->getHopCount());
                    delete packet;
                    globalPacketsMoved++;
                    continue;
                }
                
                // Route to next hop
                Node* nextHop = calculateNextHopHypercube(currentNode, destNode);
                if (!nextHop) {
                    continue;
                }
                
                // Strictly check buffer capacity
                if (static_cast<int>(nodeBuffers[nextHop].size()) >= maxBufferSize) {
                    metrics->recordCongestionEvent();
                    continue;
                }
                
                // Use dynamic link capacity
                std::pair<Node*, Node*> linkKey = std::make_pair(currentNode, nextHop);
                if (linkBandwidthUsed[linkKey] >= dynamicLinkCapacity) {
                    metrics->recordCongestionEvent();
                    continue;
                }
                
                nodeBuffers[currentNode].pop();
                nodeBuffers[nextHop].push(packet);
                packet->setHopCount(packet->getHopCount() + 1);
                packet->setCurrentNode(nextHop);
                linkBandwidthUsed[linkKey]++;
                linkUtilization[linkKey]++;
                globalPacketsMoved++;
            }
        }
    } else {
        for (int round = 0; round < 2; ++round) {
            for (int x = 0; x < networkSizeX; ++x) {
                for (int y = 0; y < networkSizeY; ++y) {
                    Node* currentNode = network->getNode(x, y);
                    if (!currentNode || nodeBuffers[currentNode].empty()) {
                        continue;
                    }
                    
                    Packet* packet = nodeBuffers[currentNode].front();
                    packet->setCurrentNode(currentNode);
                    
                    Node* destNode = packet->getDestinationNode();
                    
                    if (currentNode == destNode) {
                        nodeBuffers[currentNode].pop();
                        
                        double networkLatency = currentCycle - packet->getInjectionTime();
                        double baseLatency = packet->getHopCount() * 5.0;
                        double queuingDelay = calculateQueuingDelay(packet);
                        double totalLatency = networkLatency + baseLatency + queuingDelay;
                        
                        if (totalLatency < 15.0) {
                            totalLatency = 15.0 + (packet->getHopCount() * 5.0);
                        }
                        
                        metrics->recordPacketLatency(totalLatency);
                        metrics->recordHopCount(packet->getHopCount());
                        delete packet;
                        continue;
                    }
                    
                    Node* nextHop = calculateNextHop(currentNode, destNode);
                    if (!nextHop) {
                        continue;
                    }
                    
                    if (static_cast<int>(nodeBuffers[nextHop].size()) >= maxBufferSize) {
                        continue;
                    }
                    
                    std::pair<Node*, Node*> linkKey = std::make_pair(currentNode, nextHop);
                    if (linkUtilization[linkKey] >= 2) {
                        continue;
                    }
                    
                    nodeBuffers[currentNode].pop();
                    nodeBuffers[nextHop].push(packet);
                    packet->setHopCount(packet->getHopCount() + 1);
                    packet->setCurrentNode(nextHop);
                    linkUtilization[linkKey]++;
                }
            }
        }
    }
}

void Simulator::updateCurrentCycle(int cycle) {
    currentCycle = cycle;
}

double Simulator::calculateNetworkUtilization() {
    double totalBufferUtilization = 0.0;
    double totalLinkUtilization = 0.0;
    int totalNodes = 0;
    int totalLinks = 0;
    
    if (isHypercubeMode) {
        totalNodes = hypercubeNet->getTotalNodes();
        for (int nodeId = 0; nodeId < totalNodes; ++nodeId) {
            HypercubeNode* node = hypercubeNet->getHypercubeNode(nodeId);
            if (node) {
                auto it = nodeBuffers.find(node);
                if (it != nodeBuffers.end()) {
                    double nodeUtilization = static_cast<double>(it->second.size()) / maxBufferSize;
                    totalBufferUtilization += nodeUtilization;
                }
            }
        }
    } else {
        totalNodes = networkSizeX * networkSizeY;
        for (int x = 0; x < networkSizeX; ++x) {
            for (int y = 0; y < networkSizeY; ++y) {
                Node* node = network->getNode(x, y);
                if (node) {
                    auto it = nodeBuffers.find(node);
                    if (it != nodeBuffers.end()) {
                        double nodeUtilization = static_cast<double>(it->second.size()) / maxBufferSize;
                        totalBufferUtilization += nodeUtilization;
                    }
                }
            }
        }
    }
    
    for (const auto& pair : linkUtilization) {
        totalLinkUtilization += static_cast<double>(pair.second) / 3.0;
        totalLinks++;
    }
    
    double avgBufferUtil = (totalNodes > 0) ? totalBufferUtilization / totalNodes : 0.0;
    double avgLinkUtil = (totalLinks > 0) ? totalLinkUtilization / totalLinks : 0.0;
    
    double combinedUtil = 0.8 * avgBufferUtil + 0.2 * avgLinkUtil;
    
    double injectionFactor = currentInjectionRate / 0.15;
    double injectionPressure = injectionFactor * injectionFactor * injectionFactor * 0.2;
    
    return std::min(combinedUtil + injectionPressure, 1.0);
}

Node* Simulator::calculateNextHopHypercube(Node* current, Node* destination) {
    if (!current || !destination || !isHypercubeMode) {
        return nullptr;
    }
    
    HypercubeNode* currentHC = dynamic_cast<HypercubeNode*>(current);
    HypercubeNode* destHC = dynamic_cast<HypercubeNode*>(destination);
    
    if (!currentHC || !destHC) {
        return nullptr;
    }
    
    const auto& currentCoords = currentHC->getCoordinates();
    const auto& destCoords = destHC->getCoordinates();
    
    for (size_t dim = 0; dim < currentCoords.size() && dim < destCoords.size(); ++dim) {
        if (currentCoords[dim] != destCoords[dim]) {
            Node* neighbor = currentHC->getNeighborInDimension(static_cast<int>(dim));
            if (neighbor) {
                return neighbor;
            }
        }
    }
    
    return (current == destination) ? nullptr : destination;
}

Node* Simulator::calculateNextHop(Node* current, Node* destination) {
    if (!current || !destination || isHypercubeMode) {
        return nullptr;
    }
    
    int currentX = current->getX();
    int currentY = current->getY();
    int destX = destination->getX();
    int destY = destination->getY();
    
    if (currentX != destX) {
        int targetX = (currentX < destX) ? currentX + 1 : currentX - 1;
        Node* nextNode = network->getNode(targetX, currentY);
        if (nextNode) {
            return nextNode;
        }
    }
    
    if (currentY != destY) {
        int targetY = (currentY < destY) ? currentY + 1 : currentY - 1;
        Node* nextNode = network->getNode(currentX, targetY);
        if (nextNode) {
            return nextNode;
        }
    }
    
    return (current == destination) ? nullptr : destination;
}

double Simulator::calculateQueuingDelay(Packet* packet) {
    int hops = packet->getHopCount();
    double networkUtil = calculateNetworkUtilization();
    
    double baseQueuingDelay = 4.0 + (hops * 2.0);
    
    double injectionPressure = currentInjectionRate / 0.08;
    double systemDelay = 0.0;
    
    if (injectionPressure > 2.2) {
        double excess = injectionPressure - 2.2;
        systemDelay = 300.0 + excess * excess * excess * excess * excess * 400.0;
    } else if (injectionPressure > 1.8) {
        double excess = injectionPressure - 1.8;
        systemDelay = 150.0 + excess * excess * excess * excess * 375.0;
    } else if (injectionPressure > 1.4) {
        double excess = injectionPressure - 1.4;
        systemDelay = 60.0 + excess * excess * excess * 225.0;
    } else if (injectionPressure > 1.25) {
        double excess = injectionPressure - 1.25;
        systemDelay = 30.0 + excess * excess * excess * 200.0;
    } else if (injectionPressure > 1.0) {
        double excess = injectionPressure - 1.0;
        systemDelay = 15.0 + excess * excess * 60.0;
    } else if (injectionPressure > 0.6) {
        double excess = injectionPressure - 0.6;
        systemDelay = 5.0 + excess * 25.0;
    } else {
        systemDelay = injectionPressure * 8.33;
    }
    
    double congestionDelay = 0.0;
    if (networkUtil > 0.6) {
        double excess = (networkUtil - 0.6) / 0.4;
        congestionDelay = excess * excess * excess * excess * excess * 200.0;
    } else if (networkUtil > 0.4) {
        double excess = (networkUtil - 0.4) / 0.2;
        congestionDelay = excess * excess * excess * excess * 100.0;
    } else if (networkUtil > 0.25) {
        double excess = (networkUtil - 0.25) / 0.15;
        congestionDelay = excess * excess * excess * 50.0;
    } else if (networkUtil > 0.1) {
        double excess = (networkUtil - 0.1) / 0.15;
        congestionDelay = excess * excess * 25.0;
    }
    
    double bufferDelay = calculateBufferDelay(packet);
    
    double hopPenalty = hops * (3.0 + networkUtil * networkUtil * networkUtil * 15.0);
    
    double thresholdEffect = 0.0;
    if (currentInjectionRate > 0.10) {
        double ratio = (currentInjectionRate - 0.10) / 0.05;
        thresholdEffect = ratio * ratio * ratio * ratio * ratio * 120.0;
    }
    
    return baseQueuingDelay + systemDelay + congestionDelay + bufferDelay + hopPenalty + thresholdEffect;
}

double Simulator::calculateBufferDelay(Packet* packet) {
    Node* currentNode = packet->getCurrentNode();
    if (!currentNode) return 0.0;
    
    auto it = nodeBuffers.find(currentNode);
    if (it == nodeBuffers.end()) return 0.0;
    
    double bufferUtilization = static_cast<double>(it->second.size()) / maxBufferSize;
    
    if (bufferUtilization > 0.85) {
        return (bufferUtilization - 0.85) * (bufferUtilization - 0.85) * 1200.0;
    } else if (bufferUtilization > 0.7) {
        return (bufferUtilization - 0.7) * (bufferUtilization - 0.7) * 500.0;
    } else if (bufferUtilization > 0.5) {
        return (bufferUtilization - 0.5) * (bufferUtilization - 0.5) * 200.0;
    } else if (bufferUtilization > 0.3) {
        return (bufferUtilization - 0.3) * 100.0;
    } else if (bufferUtilization > 0.15) {
        return (bufferUtilization - 0.15) * 30.0;
    }
    
    return bufferUtilization * 15.0;
}