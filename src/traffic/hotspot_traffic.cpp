#include "hotspot_traffic.h"
#include <random>

HotspotTraffic::HotspotTraffic(int networkSize) : TrafficPattern(networkSize) {}

void HotspotTraffic::generateTraffic() {
}

std::vector<int> HotspotTraffic::generateTrafficVector() {
    std::vector<int> traffic(networkSize_);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    int hotspotNode = networkSize_ - 1;
    
    for (int i = 0; i < networkSize_; ++i) {
        if (dis(gen) < 0.3) {
            traffic[i] = hotspotNode;
        } else {
            std::uniform_int_distribution<> nodeDis(0, networkSize_ - 1);
            traffic[i] = nodeDis(gen);
        }
    }
    
    return traffic;
}