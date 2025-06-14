/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef TRAFFIC_PATTERN_H
#define TRAFFIC_PATTERN_H

#include <vector>

class TrafficPattern {
protected:
    int networkSize_;
    double injectionRate;

public:
    TrafficPattern(int networkSize) : networkSize_(networkSize), injectionRate(0.0) {}
    virtual ~TrafficPattern() {}
    
    virtual void generateTraffic() = 0;
    virtual std::vector<int> generateTrafficVector() = 0;
    virtual int generateDestination(int sourceNode) = 0;
    
    virtual double getInjectionRate() const { return injectionRate; }
    virtual void setInjectionRate(double rate) { injectionRate = rate; }
    
    int getNetworkSize() const { return networkSize_; }
};

#endif // TRAFFIC_PATTERN_H