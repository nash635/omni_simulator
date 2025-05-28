#ifndef TRANSPOSE_TRAFFIC_H
#define TRANSPOSE_TRAFFIC_H

#include "traffic_pattern.h"

class TransposeTraffic : public TrafficPattern {
public:
    TransposeTraffic(int networkSize);
    
    void generateTraffic() override;
    std::vector<int> generateTrafficVector() override;
    int generateDestination(int sourceNode) override;
    double getInjectionRate() const override;
    void setInjectionRate(double rate) override;
};

#endif // TRANSPOSE_TRAFFIC_H