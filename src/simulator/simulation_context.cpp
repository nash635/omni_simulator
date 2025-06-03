/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "simulation_context.h"
#include "../network/network_factory.h"
#include "../routing/routing_factory.h"
#include "../network/network.h"
#include "../network/hypercube_network.h"
#include "../routing/routing_algorithm.h"
#include "../utils/config.h"
#include "simulator.h"
#include <iostream>
#include <stdexcept>

SimulationContext::SimulationContext(const Config& config)
    : config_(config), initialized_(false), descriptionsGenerated_(false) {
}

SimulationContext::~SimulationContext() = default;

void SimulationContext::initialize() {
    if (initialized_) {
        return;
    }
    
    try {
        std::cout << "Initializing simulation context..." << std::endl;
        
        // Create components in the correct order
        createNetwork();
        createRoutingAlgorithm();
        createSimulator();
        
        initialized_ = true;
        std::cout << "Simulation context initialized successfully." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize simulation context: " << e.what() << std::endl;
        throw;
    }
}

Simulator* SimulationContext::getSimulator() const {
    if (!initialized_) {
        throw std::runtime_error("Simulation context not initialized");
    }
    return simulator_.get();
}

Network* SimulationContext::getNetwork() const {
    if (!initialized_) {
        throw std::runtime_error("Simulation context not initialized");
    }
    return network_.get();
}

RoutingAlgorithm* SimulationContext::getRoutingAlgorithm() const {
    if (!initialized_) {
        throw std::runtime_error("Simulation context not initialized");
    }
    return routingAlgorithm_.get();
}

const Config& SimulationContext::getConfig() const {
    return config_;
}

std::string SimulationContext::getNetworkDescription() const {
    if (!descriptionsGenerated_) {
        generateDescriptions();
    }
    return networkDescription_;
}

std::string SimulationContext::getRoutingDescription() const {
    if (!descriptionsGenerated_) {
        generateDescriptions();
    }
    return routingDescription_;
}

std::string SimulationContext::getSimulationDescription() const {
    return config_.getCompleteSimulationDescription();
}

bool SimulationContext::isInitialized() const {
    return initialized_;
}

void SimulationContext::printSetupInfo() const {
    if (!initialized_) {
        std::cout << "Simulation context not yet initialized." << std::endl;
        return;
    }
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << "Simulation Setup Information" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Network Topology: " << getNetworkDescription() << std::endl;
    std::cout << "Routing Algorithm: " << getRoutingDescription() << std::endl;
    std::cout << "Virtual Channels: " << config_.getVirtualChannels() << std::endl;
    std::cout << "Buffer Size: " << config_.getBufferSize() << " flits" << std::endl;
    std::cout << "Link Bandwidth: " << config_.getLinkBandwidth() << " flits/cycle" << std::endl;
    std::cout << "Traffic Pattern: " << config_.getTrafficPattern() << std::endl;
    std::cout << "Packet Size: " << config_.getPacketSizeFlits() << " flits" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

void SimulationContext::createNetwork() {
    NetworkFactory& factory = NetworkFactory::getInstance();
    
    const std::string& topology = config_.getNetworkTopology();
    if (!factory.isTopologySupported(topology)) {
        throw std::invalid_argument("Unsupported network topology: " + topology);
    }
    
    network_ = factory.createNetwork(config_);
    if (!network_) {
        throw std::runtime_error("Failed to create network instance");
    }
}

void SimulationContext::createRoutingAlgorithm() {
    RoutingFactory& factory = RoutingFactory::getInstance();
    
    routingAlgorithm_ = factory.createRoutingAlgorithm(network_.get(), config_);
    if (!routingAlgorithm_) {
        throw std::runtime_error("Failed to create routing algorithm instance");
    }
}

void SimulationContext::createSimulator() {
    // Check if we have a hypercube network
    HypercubeNetwork* hypercubeNet = dynamic_cast<HypercubeNetwork*>(network_.get());
    
    if (hypercubeNet) {
        // Use hypercube-specific constructor
        simulator_ = std::unique_ptr<Simulator>(new Simulator(hypercubeNet));
    } else {
        // Assume 2D mesh network
        auto size = config_.getNetworkSize2D();
        simulator_ = std::unique_ptr<Simulator>(new Simulator(size[0], size[1]));
        simulator_->setNetwork(network_.get());
    }
    
    // Set the routing algorithm
    simulator_->setRoutingAlgorithm(routingAlgorithm_.release());
    
    // Initialize the simulator
    simulator_->initializeNetwork();
}

void SimulationContext::generateDescriptions() const {
    if (descriptionsGenerated_) {
        return;
    }
    
    // Generate network description
    const std::string& topology = config_.getNetworkTopology();
    if (topology == "hypercube") {
        int dimension = config_.getHypercubeDimension();
        int totalNodes = 1 << dimension; // 2^dimension
        networkDescription_ = std::to_string(dimension) + "D-hypercube (" + 
                             std::to_string(totalNodes) + " nodes)";
    } else if (topology == "2D_mesh") {
        auto size = config_.getNetworkSize2D();
        networkDescription_ = std::to_string(size[0]) + "x" + std::to_string(size[1]) + " 2D mesh";
    } else if (topology == "3D_mesh") {
        auto size = config_.getNetworkSize3D();
        networkDescription_ = std::to_string(size[0]) + "x" + std::to_string(size[1]) + "x" + 
                             std::to_string(size[2]) + " 3D mesh";
    } else {
        networkDescription_ = topology + " network";
    }
    
    // Generate routing description
    const std::string& algorithm = config_.getRoutingAlgorithm();
    if (algorithm == "duato" && topology == "hypercube") {
        routingDescription_ = "Duato's Protocol for Hypercubes (E-cube baseline)";
    } else if (algorithm == "ecube" && topology == "hypercube") {
        routingDescription_ = "E-cube Routing for Hypercubes";
    } else if (algorithm == "duato" && topology == "2D_mesh") {
        routingDescription_ = "Duato's Protocol for 2D Mesh";
    } else {
        routingDescription_ = algorithm + " routing for " + topology;
    }
    
    descriptionsGenerated_ = true;
}
