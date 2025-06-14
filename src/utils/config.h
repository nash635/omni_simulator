/*
 * omni_simulator - Network Routing Simulator
 * Configuration Management System
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <array>

class Config {
public:
    Config();
    bool loadConfig(const std::string& filename);
    
    // Network configuration getters
    std::array<int, 2> getNetworkSize2D() const;
    std::array<int, 3> getNetworkSize3D() const;
    double getLinkBandwidth() const;
    double getLinkLatency() const;
    int getVirtualChannels() const;
    int getBufferSize() const;
    
    // Traffic configuration getters
    std::string getTrafficPattern() const;
    std::vector<double> getPacketInjectionRates() const;
    int getPacketSizeFlits() const;
    int getWarmupCycles() const;
    int getMeasurementCycles() const;
    double getHotspotRatio() const;
    std::vector<int> getHotspotNodes() const;
    
    // Routing configuration getters
    std::string getRoutingAlgorithm() const;
    int getAdaptiveThreshold() const;
    bool isDeadlockDetectionEnabled() const;
    
    // Metrics configuration getters
    bool shouldCollectLatency() const;
    bool shouldCollectThroughput() const;
    std::string getOutputFile() const;
    bool isDetailedStatsEnabled() const;
    
    // Experimental setup
    int getRunsPerInjectionRate() const;
    double getConfidenceInterval() const;
    bool isSaturationDetectionEnabled() const;
    double getLatencyThresholdMultiplier() const;
    double getThroughputDropThreshold() const;
    
    // Debug configuration
    bool isDebugEnabled() const;
    bool isRoutingVerbose() const;
    bool isPacketTraceEnabled() const;
    bool isDeadlockDetectionVerbose() const;
    bool isPerformanceCountersEnabled() const;

    // Hypercube configuration
    int getHypercubeDimension() const;
    std::string getBaselineRouting() const;
    bool isHypercubeMode() const;
    std::vector<int> getDimensionPriorities() const;
    std::string getNetworkTopology() const;
    
    // Algorithm description methods
    std::string getRoutingAlgorithmDescription() const;
    std::string getNetworkTopologyDescription() const;
    std::string getSimulationDescription() const;  // Description from config file
    std::string getCompleteSimulationDescription() const;  // Dynamically generated complete description
    
    // Simulation information methods
    std::string getSimulationName() const;
    
private:
    // Network parameters
    std::array<int, 2> networkSize2D;
    std::array<int, 3> networkSize3D;
    double linkBandwidth;
    double linkLatency;
    int virtualChannels;
    int bufferSize;
    
    // Traffic parameters
    std::string trafficPattern;
    std::vector<double> packetInjectionRates;
    int packetSizeFlits;
    int warmupCycles;
    int measurementCycles;
    double hotspotRatio;
    std::vector<int> hotspotNodes;
    
    // Routing parameters
    std::string routingAlgorithm;
    int adaptiveThreshold;
    bool deadlockDetection;
    
    // Metrics parameters
    bool collectLatency;
    bool collectThroughput;
    std::string outputFile;
    bool detailedStats;
    
    // Experimental parameters
    int runsPerInjectionRate;
    double confidenceInterval;
    bool saturationDetection;
    double latencyThresholdMultiplier;
    double throughputDropThreshold;
    
    // Debug parameters
    bool debugEnabled;
    bool routingVerbose;
    bool packetTrace;
    bool deadlockDetectionVerbose;
    bool performanceCounters;

    // Hypercube configuration
    int hypercubeDimension;
    std::string baselineRouting;
    std::vector<int> dimensionPriorities;
    std::string networkTopology;
    
    // Simulation information
    std::string simulationName;
    std::string simulationDescription;
    
    // Helper methods for parsing
    void parseNetworkConfig(const std::string& content);
    void parseTrafficConfig(const std::string& content);
    void parseRoutingConfig(const std::string& content);
    void parseMetricsConfig(const std::string& content);
    void parseExperimentalConfig(const std::string& content);
    void parseDebugConfig(const std::string& content);
    void parseSimulationConfig(const std::string& content);
};

#endif // CONFIG_H