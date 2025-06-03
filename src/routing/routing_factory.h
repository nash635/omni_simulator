/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef ROUTING_FACTORY_H
#define ROUTING_FACTORY_H

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>

class RoutingAlgorithm;
class Network;
class HypercubeNetwork;
class Config;

/**
 * @brief Factory for creating routing algorithm instances
 * 
 * This factory creates routing algorithms based on the network topology
 * and configuration. It automatically selects the best routing algorithm
 * for each network type while allowing custom configurations.
 */
class RoutingFactory {
public:
    // Type alias for routing algorithm creator function
    using RoutingCreator = std::function<std::unique_ptr<RoutingAlgorithm>(Network*, const Config&)>;
    
    /**
     * @brief Get the singleton instance of RoutingFactory
     */
    static RoutingFactory& getInstance();
    
    /**
     * @brief Register a routing algorithm for a specific network topology
     * @param algorithmName Name of the routing algorithm
     * @param topologyName Name of the network topology
     * @param creator Function that creates the routing algorithm instance
     */
    void registerRoutingAlgorithm(const std::string& algorithmName, 
                                 const std::string& topologyName, 
                                 RoutingCreator creator);
    
    /**
     * @brief Create a routing algorithm instance
     * @param network The network instance
     * @param config Configuration object
     * @return Unique pointer to the created routing algorithm
     */
    std::unique_ptr<RoutingAlgorithm> createRoutingAlgorithm(Network* network, const Config& config);
    
    /**
     * @brief Get supported algorithms for a specific topology
     * @param topologyName Name of the topology
     * @return Vector of supported algorithm names
     */
    std::vector<std::string> getSupportedAlgorithms(const std::string& topologyName) const;
    
    /**
     * @brief Check if an algorithm is supported for a topology
     * @param algorithmName Name of the algorithm
     * @param topologyName Name of the topology
     * @return True if supported, false otherwise
     */
    bool isAlgorithmSupported(const std::string& algorithmName, const std::string& topologyName) const;
    
    /**
     * @brief Get the default algorithm for a topology
     * @param topologyName Name of the topology
     * @return Name of the default algorithm
     */
    std::string getDefaultAlgorithm(const std::string& topologyName) const;
    
private:
    RoutingFactory() = default;
    ~RoutingFactory() = default;
    RoutingFactory(const RoutingFactory&) = delete;
    RoutingFactory& operator=(const RoutingFactory&) = delete;
    
    // Key format: "algorithm_topology"
    std::unordered_map<std::string, RoutingCreator> creators_;
    std::unordered_map<std::string, std::string> defaultAlgorithms_;
    
    // Helper function to create registry key
    std::string makeKey(const std::string& algorithm, const std::string& topology) const;
    
    // Initialize built-in routing algorithms
    void initializeBuiltinAlgorithms();
    
    // Helper function to get supported algorithms string
    std::string getSupportedAlgorithmsString(const std::string& topologyName) const;
};

/**
 * @brief Helper class for automatic routing algorithm registration
 */
template<typename RoutingType>
class RoutingAlgorithmRegistrar {
public:
    RoutingAlgorithmRegistrar(const std::string& algorithmName, const std::string& topologyName) {
        RoutingFactory::getInstance().registerRoutingAlgorithm(
            algorithmName, 
            topologyName,
            [](Network* network, const Config& config) -> std::unique_ptr<RoutingAlgorithm> {
                return std::unique_ptr<RoutingAlgorithm>(new RoutingType(network, config));
            }
        );
    }
};

// Macro for easy registration
#define REGISTER_ROUTING_ALGORITHM(RoutingClass, AlgorithmName, TopologyName) \
    static RoutingAlgorithmRegistrar<RoutingClass> g_##RoutingClass##_##TopologyName##_registrar(AlgorithmName, TopologyName)

#endif // ROUTING_FACTORY_H
