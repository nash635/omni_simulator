/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef SIMULATION_CONTEXT_H
#define SIMULATION_CONTEXT_H

#include <memory>
#include <string>
#include <vector>

class Network;
class RoutingAlgorithm;
class TrafficPattern;
class Simulator;
class Config;
class HypercubeNetwork;

/**
 * @brief Simulation context that encapsulates all simulation components
 * 
 * This class provides a high-level interface for setting up and managing
 * a simulation. It follows the facade pattern to hide the complexity of
 * component interaction and factory-based creation.
 */
class SimulationContext {
public:
    /**
     * @brief Create a simulation context from configuration
     * @param config Configuration object
     */
    explicit SimulationContext(const Config& config);
    
    /**
     * @brief Destructor
     */
    ~SimulationContext();
    
    /**
     * @brief Initialize the simulation context
     * 
     * This method creates the network, routing algorithm, and simulator
     * based on the configuration. It should be called after construction.
     */
    void initialize();
    
    /**
     * @brief Get the simulator instance
     * @return Pointer to the simulator
     */
    Simulator* getSimulator() const;
    
    /**
     * @brief Get the network instance
     * @return Pointer to the network
     */
    Network* getNetwork() const;
    
    /**
     * @brief Get the routing algorithm instance
     * @return Pointer to the routing algorithm
     */
    RoutingAlgorithm* getRoutingAlgorithm() const;
    
    /**
     * @brief Get the configuration
     * @return Reference to the configuration
     */
    const Config& getConfig() const;
    
    /**
     * @brief Get network description for display/logging
     * @return Human-readable network description
     */
    std::string getNetworkDescription() const;
    
    /**
     * @brief Get routing algorithm description for display/logging
     * @return Human-readable routing algorithm description
     */
    std::string getRoutingDescription() const;
    
    /**
     * @brief Get complete simulation description
     * @return Complete description including network and routing info
     */
    std::string getSimulationDescription() const;
    
    /**
     * @brief Check if the simulation is properly initialized
     * @return True if initialized, false otherwise
     */
    bool isInitialized() const;
    
    /**
     * @brief Print simulation setup information
     */
    void printSetupInfo() const;
    
private:
    const Config& config_;
    std::unique_ptr<Network> network_;
    std::unique_ptr<RoutingAlgorithm> routingAlgorithm_;
    std::unique_ptr<Simulator> simulator_;
    bool initialized_;
    
    // Helper methods
    void createNetwork();
    void createRoutingAlgorithm();
    void createSimulator();
    void setupNetworkDescription();
    void setupRoutingDescription();
    
    // Cached descriptions
    mutable std::string networkDescription_;
    mutable std::string routingDescription_;
    mutable bool descriptionsGenerated_;
    
    // Helper method to generate descriptions
    void generateDescriptions() const;
};

#endif // SIMULATION_CONTEXT_H
