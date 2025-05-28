#include "transpose_traffic.h"

TransposeTraffic::TransposeTraffic(int networkSize) : TrafficPattern(networkSize) {}

void TransposeTraffic::generateTraffic() {
}

std::vector<int> TransposeTraffic::generateTrafficVector() {
    std::vector<int> traffic(networkSize_);
    
    for (int i = 0; i < networkSize_; ++i) {
        traffic[i] = (networkSize_ - 1 - i);
    }
    
    return traffic;
}

int TransposeTraffic::generateDestination(int sourceNode) {
    return (networkSize_ - 1 - sourceNode);
}

double TransposeTraffic::getInjectionRate() const {
    return injectionRate;
}

void TransposeTraffic::setInjectionRate(double rate) {
    injectionRate = rate;
}