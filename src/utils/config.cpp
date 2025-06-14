/*
 * omni_simulator - Network Routing Simulator
 * Configuration Management System
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "config.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <cmath>
#include <sys/stat.h>

Config::Config() {
    // Set default values
    networkSize2D = {8, 8};
    networkSize3D = {4, 4, 4};
    linkBandwidth = 1.0;
    linkLatency = 1.0;
    virtualChannels = 2;
    bufferSize = 4;
    
    trafficPattern = "uniform";
    packetInjectionRates = {0.01, 0.02, 0.03, 0.04, 0.05};
    packetSizeFlits = 4;
    warmupCycles = 1000;
    measurementCycles = 10000;
    hotspotRatio = 0.3;
    hotspotNodes = {};
    
    routingAlgorithm = "duato";
    adaptiveThreshold = 2;
    deadlockDetection = true;
    
    collectLatency = true;
    collectThroughput = true;
    outputFile = "omni_simulation_results.csv";
    detailedStats = true;
    
    runsPerInjectionRate = 5;
    confidenceInterval = 0.95;
    saturationDetection = false;
    latencyThresholdMultiplier = 10.0;
    throughputDropThreshold = 0.1;
    
    // Debug defaults - all disabled
    debugEnabled = false;
    routingVerbose = false;
    packetTrace = false;
    deadlockDetectionVerbose = false;
    performanceCounters = false;

    // Hypercube defaults
    hypercubeDimension = 4;
    baselineRouting = "ecube";
    networkTopology = "2D_mesh";
    dimensionPriorities = {0, 1, 2, 3};

    // Simulation info defaults
    simulationName = "omni_simulator - Network Routing Simulation";
    simulationDescription = "Network routing simulation using various protocols";
}

bool fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

bool Config::loadConfig(const std::string& filename) {
    std::string actualFilename = filename;
    
    // Try other paths if file not found directly
    if (!fileExists(actualFilename)) {
        std::vector<std::string> searchPaths = {
            "config.json",
            "omni_simulator/config.json",
            "../config.json",
            "../../config.json",
            "../../../config.json",
            "./config.json"
        };
        
        bool found = false;
        for (const auto& path : searchPaths) {
            if (fileExists(path)) {
                actualFilename = path;
                found = true;
                break;
            }
        }
        
        if (!found) {
            std::cerr << "Config file not found in any search path" << std::endl;
            return false;
        }
    }
    
    std::ifstream file(actualFilename);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << actualFilename << std::endl;
        return false;
    }
    
    // Read entire file content
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    try {
        // Parse configuration sections
        parseSimulationConfig(content);
        parseNetworkConfig(content);
        parseTrafficConfig(content);
        parseRoutingConfig(content);
        parseMetricsConfig(content);
        parseExperimentalConfig(content);
        parseDebugConfig(content);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing config file: " << e.what() << std::endl;
        return false;
    }
}

void Config::parseNetworkConfig(const std::string& content) {
    // Parse network configuration
    std::regex width_regex("\"width\":\\s*(\\d+)");
    std::regex height_regex("\"height\":\\s*(\\d+)");
    std::regex bandwidth_regex("\"link_bandwidth\":\\s*([\\d.]+)");
    std::regex latency_regex("\"link_latency\":\\s*([\\d.]+)");
    std::regex vc_regex("\"virtual_channels\":\\s*(\\d+)");
    std::regex buffer_regex("\"buffer_size\":\\s*(\\d+)");
    
    std::smatch match;
    if (std::regex_search(content, match, width_regex)) {
        networkSize2D[0] = std::stoi(match[1].str());
    }
    if (std::regex_search(content, match, height_regex)) {
        networkSize2D[1] = std::stoi(match[1].str());
    }
    if (std::regex_search(content, match, bandwidth_regex)) {
        linkBandwidth = std::stod(match[1].str());
    }
    // Ensure minimum link latency of 1.0
    if (std::regex_search(content, match, std::regex(R"("link_latency":\s*([0-9.]+))"))) {
        linkLatency = std::stod(match[1].str());
        if (linkLatency < 1.0) {
            linkLatency = 5.0;
        }
    }
    if (std::regex_search(content, match, vc_regex)) {
        virtualChannels = std::stoi(match[1].str());
    }
    if (std::regex_search(content, match, buffer_regex)) {
        bufferSize = std::stoi(match[1].str());
    }
    
    // Parse topology configuration
    std::regex topology_regex("\"topology\":\\s*\"([^\"]+)\"");
    std::regex hypercube_dim_regex("\"hypercube_dimension\":\\s*(\\d+)");
    std::regex baseline_routing_regex("\"baseline_routing\":\\s*\"([^\"]+)\"");
    
    if (std::regex_search(content, match, topology_regex)) {
        networkTopology = match[1].str();
    }
    if (std::regex_search(content, match, hypercube_dim_regex)) {
        hypercubeDimension = std::stoi(match[1].str());
    }
    if (std::regex_search(content, match, baseline_routing_regex)) {
        baselineRouting = match[1].str();
    }
}

void Config::parseTrafficConfig(const std::string& content) {
    // Parse traffic pattern
    std::regex pattern_regex("\"pattern\":\\s*\"([^\"]+)\"");
    std::regex packet_size_regex("\"packet_size_flits\":\\s*(\\d+)");
    std::regex warmup_regex("\"warmup_cycles\":\\s*(\\d+)");
    std::regex measurement_regex("\"measurement_cycles\":\\s*(\\d+)");
    
    // Fix injection rate parsing - support decimals
    std::regex injection_rates_regex("\"packet_injection_rates\":\\s*\\[([\\d.,\\s]+)\\]");
    
    std::smatch match;
    if (std::regex_search(content, match, pattern_regex)) {
        trafficPattern = match[1].str();
    }
    
    if (std::regex_search(content, match, packet_size_regex)) {
        packetSizeFlits = std::stoi(match[1].str());
    }
    
    if (std::regex_search(content, match, warmup_regex)) {
        warmupCycles = std::stoi(match[1].str());
    }
    
    if (std::regex_search(content, match, measurement_regex)) {
        measurementCycles = std::stoi(match[1].str());
    }
    
    // Parse injection rates array
    if (std::regex_search(content, match, injection_rates_regex)) {
        std::string rates_str = match[1].str();
        
        packetInjectionRates.clear();
        std::stringstream ss(rates_str);
        std::string rate;
        
        while (std::getline(ss, rate, ',')) {
            // Remove whitespace
            rate.erase(0, rate.find_first_not_of(" \t"));
            rate.erase(rate.find_last_not_of(" \t") + 1);
            
            if (!rate.empty()) {
                try {
                    double rateValue = std::stod(rate);
                    packetInjectionRates.push_back(rateValue);
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing injection rate '" << rate << "': " << e.what() << std::endl;
                }
            }
        }
    }
}

void Config::parseDebugConfig(const std::string& content) {
    // Parse debug configuration
    std::regex debug_enabled_regex("\"enabled\":\\s*(true|false)");
    std::regex routing_verbose_regex("\"routing_verbose\":\\s*(true|false)");
    std::regex packet_trace_regex("\"packet_trace\":\\s*(true|false)");
    std::regex deadlock_verbose_regex("\"deadlock_detection_verbose\":\\s*(true|false)");
    std::regex performance_counters_regex("\"performance_counters\":\\s*(true|false)");
    
    std::smatch match;
    if (std::regex_search(content, match, debug_enabled_regex)) {
        debugEnabled = (match[1].str() == "true");
    }
    if (std::regex_search(content, match, routing_verbose_regex)) {
        routingVerbose = (match[1].str() == "true");
    }
    if (std::regex_search(content, match, packet_trace_regex)) {
        packetTrace = (match[1].str() == "true");
    }
    if (std::regex_search(content, match, deadlock_verbose_regex)) {
        deadlockDetectionVerbose = (match[1].str() == "true");
    }
    if (std::regex_search(content, match, performance_counters_regex)) {
        performanceCounters = (match[1].str() == "true");
    }
}

// Add simulation info parsing
void Config::parseSimulationConfig(const std::string& content) {
    std::regex name_regex("\"name\":\\s*\"([^\"]+)\"");
    std::regex description_regex("\"description\":\\s*\"([^\"]+)\"");
    
    std::smatch match;
    if (std::regex_search(content, match, name_regex)) {
        simulationName = match[1].str();
    }
    
    if (std::regex_search(content, match, description_regex)) {
        simulationDescription = match[1].str();
    }
}

// Getter method implementations
std::array<int, 2> Config::getNetworkSize2D() const {
    return networkSize2D;
}

std::array<int, 3> Config::getNetworkSize3D() const {
    return networkSize3D;
}

double Config::getLinkBandwidth() const {
    return linkBandwidth;
}

double Config::getLinkLatency() const {
    return linkLatency;
}

std::vector<double> Config::getPacketInjectionRates() const {
    return packetInjectionRates;
}

int Config::getPacketSizeFlits() const {
    return packetSizeFlits;
}

int Config::getWarmupCycles() const {
    return warmupCycles;
}

int Config::getMeasurementCycles() const {
    return measurementCycles;
}

std::string Config::getTrafficPattern() const {
    return trafficPattern;
}

bool Config::shouldCollectLatency() const {
    return collectLatency;
}

bool Config::shouldCollectThroughput() const {
    return collectThroughput;
}

// Debug getter methods
bool Config::isDebugEnabled() const {
    return debugEnabled;
}

bool Config::isRoutingVerbose() const {
    return debugEnabled && routingVerbose;
}

bool Config::isPacketTraceEnabled() const {
    return debugEnabled && packetTrace;
}

bool Config::isDeadlockDetectionVerbose() const {
    return debugEnabled && deadlockDetectionVerbose;
}

bool Config::isPerformanceCountersEnabled() const {
    return debugEnabled && performanceCounters;
}

std::string Config::getOutputFile() const {
    return outputFile;
}

bool Config::isDetailedStatsEnabled() const {
    return detailedStats;
}

int Config::getRunsPerInjectionRate() const {
    return runsPerInjectionRate;
}

double Config::getConfidenceInterval() const {
    return confidenceInterval;
}

bool Config::isSaturationDetectionEnabled() const {
    return saturationDetection;
}

double Config::getLatencyThresholdMultiplier() const {
    return latencyThresholdMultiplier;
}

double Config::getThroughputDropThreshold() const {
    return throughputDropThreshold;
}

int Config::getVirtualChannels() const {
    return virtualChannels;
}

int Config::getBufferSize() const {
    return bufferSize;
}

double Config::getHotspotRatio() const {
    return hotspotRatio;
}

std::vector<int> Config::getHotspotNodes() const {
    return hotspotNodes;
}

std::string Config::getRoutingAlgorithm() const {
    return routingAlgorithm;
}

int Config::getAdaptiveThreshold() const {
    return adaptiveThreshold;
}

bool Config::isDeadlockDetectionEnabled() const {
    return deadlockDetection;
}

// Add implementation
int Config::getHypercubeDimension() const {
    return hypercubeDimension;
}

std::string Config::getBaselineRouting() const {
    return baselineRouting;
}

bool Config::isHypercubeMode() const {
    return networkTopology == "hypercube";
}

std::vector<int> Config::getDimensionPriorities() const {
    return dimensionPriorities;
}

std::string Config::getNetworkTopology() const {
    return networkTopology;
}

// Simplified implementation using defaults
void Config::parseRoutingConfig(const std::string& content) {
    (void)content;
}

void Config::parseMetricsConfig(const std::string& content) {
    (void)content;
}

void Config::parseExperimentalConfig(const std::string& content) {
    (void)content;
}

// Ensure method implementations match header declarations

// Detailed description methods for Config class
std::string Config::getRoutingAlgorithmDescription() const {
    if (routingAlgorithm == "duato") {
        if (networkTopology == "hypercube") {
            return "Duato's Deadlock-Free Protocol for Hypercubes (E-cube baseline)";
        } else if (networkTopology == "2D_mesh") {
            return "Duato's Deadlock-Free Protocol for 2D Mesh";
        } else {
            return "Duato's Deadlock-Free Routing Protocol";
        }
    } else if (routingAlgorithm == "ecube") {
        if (networkTopology == "hypercube") {
            return "E-cube Routing Algorithm for Hypercubes";
        } else {
            return "E-cube Routing Algorithm";
        }
    } else if (routingAlgorithm == "adaptive") {
        return "Adaptive Routing Algorithm";
    } else {
        return "Custom Routing Algorithm (" + routingAlgorithm + ")";
    }
}

std::string Config::getNetworkTopologyDescription() const {
    if (networkTopology == "2D_mesh") {
        auto size = getNetworkSize2D();
        return std::to_string(size[0]) + "x" + std::to_string(size[1]) + " 2D Mesh";
    } else if (networkTopology == "hypercube") {
        int dimension = getHypercubeDimension();
        int totalNodes = static_cast<int>(std::pow(2, dimension));
        return std::to_string(dimension) + "D Hypercube (" + std::to_string(totalNodes) + " nodes)";
    } else if (networkTopology == "3D_mesh") {
        auto size = getNetworkSize3D();
        return std::to_string(size[0]) + "x" + std::to_string(size[1]) + "x" + std::to_string(size[2]) + " 3D Mesh";
    } else {
        return "Custom Network Topology (" + networkTopology + ")";
    }
}

// Implement getSimulationDescription only once
std::string Config::getSimulationDescription() const {
    return simulationDescription;
}

// Implement getCompleteSimulationDescription method in Config class

// Complete simulation description (combines algorithm and network topology)
std::string Config::getCompleteSimulationDescription() const {
    return getRoutingAlgorithmDescription() + " for " + getNetworkTopologyDescription();
}

// Implement simulation name getter method
std::string Config::getSimulationName() const {
    return simulationName;
}