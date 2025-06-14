/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "hypercube_uniform_traffic.h"
#include <random>
#include <cmath>

HypercubeUniformTraffic::HypercubeUniformTraffic(int dimension) 
    : TrafficPattern(static_cast<int>(std::pow(2, dimension))), dimension(dimension), totalNodes(static_cast<int>(std::pow(2, dimension))) {
}

HypercubeUniformTraffic::~HypercubeUniformTraffic() {
}

void HypercubeUniformTraffic::generateTraffic() {
}

std::vector<int> HypercubeUniformTraffic::generateTrafficVector() {
    std::vector<int> traffic(totalNodes);
    
    for (int i = 0; i < totalNodes; ++i) {
        traffic[i] = generateDestination(i);
    }
    
    return traffic;
}

int HypercubeUniformTraffic::generateDestination(int sourceNode) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, totalNodes - 1);
    
    int destination;
    do {
        destination = dis(gen);
    } while (destination == sourceNode);
    
    return destination;
}

double HypercubeUniformTraffic::getInjectionRate() const {
    return injectionRate;
}

void HypercubeUniformTraffic::setInjectionRate(double rate) {
    injectionRate = rate;
}