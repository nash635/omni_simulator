#ifndef HOTSPOT_TRAFFIC_H
#define HOTSPOT_TRAFFIC_H

#include "traffic_pattern.h"

class HotspotTraffic : public TrafficPattern {
public:
    HotspotTraffic(int networkSize);
    void generateTraffic() override;
    std::vector<int> generateTrafficVector() override;
};

#endif // HOTSPOT_TRAFFIC_H