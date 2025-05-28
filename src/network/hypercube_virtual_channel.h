/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef HYPERCUBE_VIRTUAL_CHANNEL_H
#define HYPERCUBE_VIRTUAL_CHANNEL_H

#include "virtual_channel.h"
#include <vector>

enum class HypercubeVirtualChannel {
    DETERMINISTIC_LOW = 0,
    DETERMINISTIC_HIGH = 1,
    ADAPTIVE = 2
};

class VirtualChannelDependencyGraph {
public:
    bool wouldCreateCycle(int fromVC, int toVC, int dimension) const;
    void addDependency(int fromVC, int toVC, int dimension);
    bool isValidTransition(VirtualChannel from, VirtualChannel to, int dimension) const;
    
private:
    std::vector<std::vector<bool>> dependencies;
};

#endif // HYPERCUBE_VIRTUAL_CHANNEL_H