/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef HYPERCUBE_UNIFORM_TRAFFIC_H
#define HYPERCUBE_UNIFORM_TRAFFIC_H

#include "traffic_pattern.h"

class HypercubeUniformTraffic : public TrafficPattern {
private:
    int dimension;
    int totalNodes;

public:
    HypercubeUniformTraffic(int dimension);
    ~HypercubeUniformTraffic();
    
    void generateTraffic() override;
    std::vector<int> generateTrafficVector() override;
    int generateDestination(int sourceNode) override;
    double getInjectionRate() const override;
    void setInjectionRate(double rate) override;
};

#endif // HYPERCUBE_UNIFORM_TRAFFIC_H