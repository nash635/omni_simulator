/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef NETWORK_FACTORY_H
#define NETWORK_FACTORY_H

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>

class Network;
class Config;

/**
 * @brief Factory for creating network instances based on configuration
 * 
 * This factory implements the Open-Closed Principle by allowing new network
 * types to be registered without modifying existing code. Each network type
 * has its own creator function that handles the specific construction logic.
 */
class NetworkFactory {
public:
    // Type alias for network creator function
    using NetworkCreator = std::function<std::unique_ptr<Network>(const Config&)>;
    
    /**
     * @brief Get the singleton instance of NetworkFactory
     */
    static NetworkFactory& getInstance();
    
    /**
     * @brief Register a new network type with its creator function
     * @param topologyName The name of the topology (e.g., "2D_mesh", "hypercube")
     * @param creator Function that creates the network instance
     */
    void registerNetworkType(const std::string& topologyName, NetworkCreator creator);
    
    /**
     * @brief Create a network instance based on configuration
     * @param config Configuration object containing network parameters
     * @return Unique pointer to the created network
     * @throws std::invalid_argument if topology is not supported
     */
    std::unique_ptr<Network> createNetwork(const Config& config);
    
    /**
     * @brief Get list of supported network topologies
     * @return Vector of supported topology names
     */
    std::vector<std::string> getSupportedTopologies() const;
    
    /**
     * @brief Check if a topology is supported
     * @param topologyName Name of the topology to check
     * @return True if topology is supported, false otherwise
     */
    bool isTopologySupported(const std::string& topologyName) const;
    
private:
    NetworkFactory() = default;
    ~NetworkFactory() = default;
    NetworkFactory(const NetworkFactory&) = delete;
    NetworkFactory& operator=(const NetworkFactory&) = delete;
    
    // Registry of network creators
    std::unordered_map<std::string, NetworkCreator> creators_;
    
    // Initialize built-in network types
    void initializeBuiltinTypes();
    
    // Helper method to get supported topologies as string
    std::string getSupportedTopologiesString() const;
};

/**
 * @brief Helper class for automatic network type registration
 * 
 * This class allows network types to register themselves automatically
 * when the program starts, following the self-registering factory pattern.
 */
template<typename NetworkType>
class NetworkTypeRegistrar {
public:
    NetworkTypeRegistrar(const std::string& topologyName) {
        NetworkFactory::getInstance().registerNetworkType(
            topologyName, 
            [](const Config& config) -> std::unique_ptr<Network> {
                return std::unique_ptr<Network>(new NetworkType(config));
            }
        );
    }
};

// Macro for easy registration
#define REGISTER_NETWORK_TYPE(NetworkClass, TopologyName) \
    static NetworkTypeRegistrar<NetworkClass> g_##NetworkClass##_registrar(TopologyName)

#endif // NETWORK_FACTORY_H
