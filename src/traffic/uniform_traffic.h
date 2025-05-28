#ifndef UNIFORM_TRAFFIC_H
#define UNIFORM_TRAFFIC_H

#include "traffic_pattern.h"

class UniformTraffic : public TrafficPattern {
public:
    UniformTraffic(int networkSize);
    
    void generateTraffic() override;
    std::vector<int> generateTrafficVector() override;
    int generateDestination(int sourceNode) override;
    double getInjectionRate() const override;
    void setInjectionRate(double rate) override;
};

#endif // UNIFORM_TRAFFIC_H