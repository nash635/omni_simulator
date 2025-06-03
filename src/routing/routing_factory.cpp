/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "routing_factory.h"
#include "routing_algorithm.h"
#include "duato_protocol.h"
#include "ecube_routing.h"
#include "duato_hypercube_protocol.h"
#include "../network/network.h"
#include "../network/hypercube_network.h"
#include "../utils/config.h"
#include <stdexcept>
#include <iostream>

RoutingFactory& RoutingFactory::getInstance() {
    static RoutingFactory instance;
    static bool initialized = false;
    
    if (!initialized) {
        instance.initializeBuiltinAlgorithms();
        initialized = true;
    }
    
    return instance;
}

void RoutingFactory::registerRoutingAlgorithm(const std::string& algorithmName, 
                                             const std::string& topologyName, 
                                             RoutingCreator creator) {
    std::string key = makeKey(algorithmName, topologyName);
    creators_[key] = creator;
    std::cout << "Registered routing algorithm: " << algorithmName << " for topology: " << topologyName << std::endl;
}

std::unique_ptr<RoutingAlgorithm> RoutingFactory::createRoutingAlgorithm(Network* network, const Config& config) {
    const std::string& topology = config.getNetworkTopology();
    std::string algorithm = config.getRoutingAlgorithm();
    
    // First try to find the requested algorithm
    std::string key = makeKey(algorithm, topology);
    auto it = creators_.find(key);
    
    // If not found, try the default algorithm for this topology
    if (it == creators_.end()) {
        std::string defaultAlg = getDefaultAlgorithm(topology);
        if (!defaultAlg.empty()) {
            std::cout << "Warning: Algorithm '" << algorithm 
                      << "' not supported for topology '" << topology 
                      << "'. Using default: " << defaultAlg << std::endl;
            algorithm = defaultAlg;
            key = makeKey(algorithm, topology);
            it = creators_.find(key);
        }
    }
    
    if (it == creators_.end()) {
        throw std::invalid_argument("No suitable routing algorithm found for topology: " + topology + 
                                   " with algorithm: " + algorithm + 
                                   ". Supported algorithms: " + getSupportedAlgorithmsString(topology));
    }
    
    std::cout << "Creating routing algorithm: " << algorithm << " for topology: " << topology << std::endl;
    return it->second(network, config);
}

std::vector<std::string> RoutingFactory::getSupportedAlgorithms(const std::string& topologyName) const {
    std::vector<std::string> algorithms;
    
    for (const auto& pair : creators_) {
        const std::string& key = pair.first;
        size_t pos = key.find('_');
        if (pos != std::string::npos) {
            std::string topology = key.substr(pos + 1);
            if (topology == topologyName) {
                std::string algorithm = key.substr(0, pos);
                algorithms.push_back(algorithm);
            }
        }
    }
    
    return algorithms;
}

bool RoutingFactory::isAlgorithmSupported(const std::string& algorithmName, const std::string& topologyName) const {
    std::string key = makeKey(algorithmName, topologyName);
    return creators_.find(key) != creators_.end();
}

std::string RoutingFactory::getDefaultAlgorithm(const std::string& topologyName) const {
    auto it = defaultAlgorithms_.find(topologyName);
    return (it != defaultAlgorithms_.end()) ? it->second : "";
}

std::string RoutingFactory::makeKey(const std::string& algorithm, const std::string& topology) const {
    return algorithm + "_" + topology;
}

std::string RoutingFactory::getSupportedAlgorithmsString(const std::string& topologyName) const {
    auto algorithms = getSupportedAlgorithms(topologyName);
    std::string result;
    
    for (size_t i = 0; i < algorithms.size(); ++i) {
        result += algorithms[i];
        if (i < algorithms.size() - 1) {
            result += ", ";
        }
    }
    
    return result;
}

void RoutingFactory::initializeBuiltinAlgorithms() {
    // Register routing algorithms for 2D Mesh
    registerRoutingAlgorithm("duato", "2D_mesh", 
        [](Network* network, const Config& config) -> std::unique_ptr<RoutingAlgorithm> {
            return std::unique_ptr<RoutingAlgorithm>(new DuatoProtocol(network, &config));
        }
    );
    
    // Register routing algorithms for Hypercube
    registerRoutingAlgorithm("ecube", "hypercube", 
        [](Network* network, const Config& config) -> std::unique_ptr<RoutingAlgorithm> {
            HypercubeNetwork* hypercubeNet = dynamic_cast<HypercubeNetwork*>(network);
            if (!hypercubeNet) {
                throw std::invalid_argument("E-cube routing requires HypercubeNetwork");
            }
            return std::unique_ptr<RoutingAlgorithm>(new EcubeRouting(hypercubeNet));
        }
    );
    
    registerRoutingAlgorithm("duato", "hypercube", 
        [](Network* network, const Config& config) -> std::unique_ptr<RoutingAlgorithm> {
            HypercubeNetwork* hypercubeNet = dynamic_cast<HypercubeNetwork*>(network);
            if (!hypercubeNet) {
                throw std::invalid_argument("Duato hypercube protocol requires HypercubeNetwork");
            }
            return std::unique_ptr<RoutingAlgorithm>(new DuatoHypercubeProtocol(hypercubeNet, &config));
        }
    );
    
    // Register routing algorithms for 3D Mesh (for future extension)
    registerRoutingAlgorithm("duato", "3D_mesh", 
        [](Network* network, const Config& config) -> std::unique_ptr<RoutingAlgorithm> {
            // For now, use 2D Duato as placeholder
            return std::unique_ptr<RoutingAlgorithm>(new DuatoProtocol(network, &config));
        }
    );
    
    // Set default algorithms
    defaultAlgorithms_["2D_mesh"] = "duato";
    defaultAlgorithms_["hypercube"] = "ecube";
    defaultAlgorithms_["3D_mesh"] = "duato";
}
