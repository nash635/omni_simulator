/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "network_factory.h"
#include "network.h"
#include "hypercube_network.h"
#include "../utils/config.h"
#include <stdexcept>
#include <iostream>

NetworkFactory& NetworkFactory::getInstance() {
    static NetworkFactory instance;
    static bool initialized = false;
    
    if (!initialized) {
        instance.initializeBuiltinTypes();
        initialized = true;
    }
    
    return instance;
}

void NetworkFactory::registerNetworkType(const std::string& topologyName, NetworkCreator creator) {
    creators_[topologyName] = creator;
    std::cout << "Registered network topology: " << topologyName << std::endl;
}

std::unique_ptr<Network> NetworkFactory::createNetwork(const Config& config) {
    const std::string& topology = config.getNetworkTopology();
    
    auto it = creators_.find(topology);
    if (it == creators_.end()) {
        throw std::invalid_argument("Unsupported network topology: " + topology + 
                                   ". Supported topologies: " + getSupportedTopologiesString());
    }
    
    std::cout << "Creating network with topology: " << topology << std::endl;
    return it->second(config);
}

std::vector<std::string> NetworkFactory::getSupportedTopologies() const {
    std::vector<std::string> topologies;
    topologies.reserve(creators_.size());
    
    for (const auto& pair : creators_) {
        topologies.push_back(pair.first);
    }
    
    return topologies;
}

bool NetworkFactory::isTopologySupported(const std::string& topologyName) const {
    return creators_.find(topologyName) != creators_.end();
}

std::string NetworkFactory::getSupportedTopologiesString() const {
    auto topologies = getSupportedTopologies();
    std::string result;
    
    for (size_t i = 0; i < topologies.size(); ++i) {
        result += topologies[i];
        if (i < topologies.size() - 1) {
            result += ", ";
        }
    }
    
    return result;
}

void NetworkFactory::initializeBuiltinTypes() {
    // Register 2D Mesh network
    registerNetworkType("2D_mesh", [](const Config& config) -> std::unique_ptr<Network> {
        auto size = config.getNetworkSize2D();
        auto network = std::unique_ptr<Network>(new Network(size[0], size[1]));
        network->initializeTopology();
        return network;
    });
    
    // Register Hypercube network
    registerNetworkType("hypercube", [](const Config& config) -> std::unique_ptr<Network> {
        int dimension = config.getHypercubeDimension();
        auto network = std::unique_ptr<Network>(new HypercubeNetwork(dimension));
        network->initializeTopology();
        return network;
    });
    
    // Register 3D Mesh network (for future extension)
    registerNetworkType("3D_mesh", [](const Config& config) -> std::unique_ptr<Network> {
        auto size = config.getNetworkSize3D();
        // For now, create a 2D network as placeholder
        auto network = std::unique_ptr<Network>(new Network(size[0] * size[1], size[2]));
        network->initializeTopology();
        return network;
    });
}
