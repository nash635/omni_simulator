#include "traffic/uniform_traffic.h"
#include <random>
#include <algorithm>

UniformTraffic::UniformTraffic(int networkSize) : TrafficPattern(networkSize) {}

void UniformTraffic::generateTraffic() {
}

std::vector<int> UniformTraffic::generateTrafficVector() {
    std::vector<int> traffic(networkSize_);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, networkSize_ - 1);
    
    for (int i = 0; i < networkSize_; ++i) {
        traffic[i] = dis(gen);
    }
    
    return traffic;
}

int UniformTraffic::generateDestination(int sourceNode) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, networkSize_ - 1);
    
    int destination;
    do {
        destination = dis(gen);
    } while (destination == sourceNode);
    
    return destination;
}

double UniformTraffic::getInjectionRate() const {
    return injectionRate;
}

void UniformTraffic::setInjectionRate(double rate) {
    injectionRate = rate;
}