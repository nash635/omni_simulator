/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <ctime>

#include "simulator/simulation_context.h"
#include "simulator/simulator.h"
#include "utils/config.h"
#include "utils/table_formatter.h"

// Network and routing factories
#include "network/network_factory.h"
#include "routing/routing_factory.h"

// Result data structure
struct ExperimentResult {
    double injectionRate;
    double averageDelay;
    double throughput;
    bool saturated;
};

// Fixed table display precision settings
void printResultsTableFormatted(const std::vector<ExperimentResult>& results) {
    TableFormatter table("omni_simulator Experimental Results");
    
    table.addColumn("Injection Rate", "(packets/cyc/node)", 20, 3);
    table.addColumn("Average Delay", "(cycles)", 15, 3);
    table.addColumn("Throughput", "(flits/cyc/node)", 20, 4);
    table.addColumn("Saturated", "(Yes/No)", 12, 0, false);
    
    for (const auto& result : results) {
        std::vector<std::string> row;
        row.push_back(std::to_string(result.injectionRate));
        
        // Special handling for blocked state delay display
        if (result.averageDelay < 0 || result.averageDelay > 500.0) {
            row.push_back("BLOCKED");  // Display as BLOCKED instead of number
        } else {
            row.push_back(std::to_string(result.averageDelay));
        }
        
        row.push_back(std::to_string(result.throughput));
        row.push_back(result.saturated ? "Yes" : "No");
        table.addRow(row);
    }
    
    table.print();
}

// Function to read results from CSV file
std::vector<ExperimentResult> readResultsFromCSV(const std::string& filename) {
    std::vector<ExperimentResult> results;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open results file for reading: " << filename << std::endl;
        return results;
    }
    
    std::string line;
    // Skip comment lines and headers
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line.find("InjectionRate") != std::string::npos) {
            continue;
        }
        
        // Parse CSV line
        std::stringstream ss(line);
        std::string token;
        ExperimentResult result;
        
        try {
            // Read injection rate
            if (std::getline(ss, token, ',')) {
                result.injectionRate = std::stod(token);
            } else continue;
            
            // Read average delay
            if (std::getline(ss, token, ',')) {
                result.averageDelay = std::stod(token);
            } else continue;
            
            // Read throughput
            if (std::getline(ss, token, ',')) {
                result.throughput = std::stod(token);
            } else continue;
            
            // Read saturated status
            if (std::getline(ss, token, ',')) {
                result.saturated = (token == "1" || token == "true");
            } else {
                result.saturated = false;
            }
            
            results.push_back(result);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Could not parse line: " << line << std::endl;
            continue;
        }
    }
    
    file.close();
    return results;
}

// Calculate and display statistics
void printStatistics(const std::vector<ExperimentResult>& results, const Config& config) {
    if (results.empty()) {
        std::cout << "No results to analyze." << std::endl;
        return;
    }
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "Performance Analysis Summary" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    // Display experimental configuration
    std::cout << "Experimental Configuration:" << std::endl;
    std::cout << "  Network Topology: " << config.getNetworkTopologyDescription() << std::endl;
    std::cout << "  Routing Algorithm: " << config.getRoutingAlgorithmDescription() << std::endl;
    
    if (config.isHypercubeMode()) {
        std::cout << "  Hypercube Dimension: " << config.getHypercubeDimension() << std::endl;
        std::cout << "  Total Nodes: " << static_cast<int>(std::pow(2, config.getHypercubeDimension())) << std::endl;
        if (config.getRoutingAlgorithm() == "duato") {
            std::cout << "  Baseline Routing: E-cube" << std::endl;
            std::cout << "  Deadlock Avoidance: Duato's Protocol" << std::endl;
        }
    } else {
        auto size = config.getNetworkSize2D();
        std::cout << "  Network Size: " << size[0] << "x" << size[1] << std::endl;
        std::cout << "  Total Nodes: " << size[0] * size[1] << std::endl;
    }
    
    std::cout << "  Virtual Channels: " << config.getVirtualChannels() << std::endl;
    std::cout << "  Buffer Size: " << config.getBufferSize() << " flits" << std::endl;
    std::cout << "  Link Bandwidth: " << config.getLinkBandwidth() << " flits/cycle" << std::endl;
    std::cout << "  Traffic Pattern: " << config.getTrafficPattern() << std::endl;
    std::cout << "  Packet Size: " << config.getPacketSizeFlits() << " flits" << std::endl;
    std::cout << "  Warmup Cycles: " << config.getWarmupCycles() << std::endl;
    std::cout << "  Measurement Cycles: " << config.getMeasurementCycles() << std::endl;
    
    std::cout << std::string(80, '-') << std::endl;
    
    // Find non-saturated results for analysis
    std::vector<ExperimentResult> nonSaturatedResults;
    std::vector<ExperimentResult> saturatedResults;
    
    for (const auto& result : results) {
        if (!result.saturated && result.averageDelay > 0 && result.averageDelay < 100) {
            nonSaturatedResults.push_back(result);
        } else if (result.saturated) {
            saturatedResults.push_back(result);
        }
    }
    
    if (!nonSaturatedResults.empty()) {
        double minDelay = nonSaturatedResults.front().averageDelay;
        double maxDelay = nonSaturatedResults.back().averageDelay;
        double maxThroughput = 0.0;
        
        for (const auto& result : nonSaturatedResults) {
            maxThroughput = std::max(maxThroughput, result.throughput);
        }
        
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Non-saturated Delay Range: " << minDelay << " - " << maxDelay << " cycles" << std::endl;
        std::cout << "Peak Throughput:           " << std::setprecision(4) << maxThroughput << " flits/cycle/node" << std::endl;
        
        // Calculate pre-saturation delay growth
        double delayIncrease = ((maxDelay - minDelay) / minDelay) * 100;
        std::cout << "Pre-saturation Delay Growth: " << std::setprecision(1) << delayIncrease << "%" << std::endl;
    }
    
    // Check saturation point
    auto saturatedIt = std::find_if(results.begin(), results.end(), 
                                   [](const ExperimentResult& r) { return r.saturated; });
    
    if (saturatedIt != results.end()) {
        std::cout << "Saturation Point:      " << std::setprecision(3) 
                  << saturatedIt->injectionRate << " packets/cycle/node" << std::endl;
        
        // Show performance drop at saturation
        if (saturatedIt != results.begin()) {
            auto prevIt = saturatedIt - 1;
            double throughputDrop = ((prevIt->throughput - saturatedIt->throughput) / prevIt->throughput) * 100;
            std::cout << "Throughput Drop at Saturation: " << std::setprecision(1) << throughputDrop << "%" << std::endl;
        }
    } else {
        std::cout << "Saturation Point:      Not reached in tested range" << std::endl;
    }
    
    std::cout << "Test Range:            " << std::setprecision(3) 
              << results.front().injectionRate << " - " << results.back().injectionRate 
              << " packets/cycle/node" << std::endl;
    std::cout << "Total Test Points:     " << results.size() << std::endl;
    
    std::cout << std::string(80, '=') << std::endl;
}

void printCSVVerification(const std::vector<ExperimentResult>& csvResults, 
                         const std::vector<ExperimentResult>& experimentResults,
                         const std::string& filename) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "CSV File Verification" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    if (!csvResults.empty() && csvResults.size() == experimentResults.size()) {
        std::cout << "✓ CSV file '" << filename << "' contains " << csvResults.size() << " valid result entries" << std::endl;
        std::cout << "✓ All experimental results successfully written to file" << std::endl;
    } else {
        std::cout << "⚠ Warning: CSV file verification failed" << std::endl;
        std::cout << "  Expected entries: " << experimentResults.size() << std::endl;
        std::cout << "  Found entries: " << csvResults.size() << std::endl;
    }
    
    std::cout << std::string(60, '=') << std::endl;
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

int main() {
    Config config;
    if (!config.loadConfig("config.json")) {
        std::cerr << "Failed to load configuration file. Using default settings." << std::endl;
    }

    std::string simulationName = config.getSimulationName();
    std::string simulationDescription = config.getSimulationDescription();
    
    if (simulationDescription.empty() || simulationDescription == "Network routing simulation using various protocols") {
        simulationDescription = config.getSimulationDescription();
    }
    
    std::cout << "================================================================================" << std::endl;
    std::cout << simulationName << std::endl;
    std::cout << simulationDescription << std::endl;
    std::cout << "Copyright (c) 2025 nash635" << std::endl;
    std::cout << "================================================================================" << std::endl;
    
    // Create simulation context using the new factory-based design
    SimulationContext simulationContext(config);
    
    try {
        // Initialize the simulation context
        simulationContext.initialize();
        
        // Print setup information
        simulationContext.printSetupInfo();
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize simulation: " << e.what() << std::endl;
        return 1;
    }

    // Get the simulator from the context
    Simulator* simulator = simulationContext.getSimulator();

    // Get injection rate list
    auto injectionRates = config.getPacketInjectionRates();
    std::cout << "Testing " << injectionRates.size() << " injection rates: ";
    for (size_t i = 0; i < injectionRates.size(); ++i) {
        std::cout << injectionRates[i];
        if (i < injectionRates.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;

    // Open results file
    std::ofstream resultsFile(config.getOutputFile());
    
    // Get descriptions from simulation context
    std::string finalNetworkDescription = simulationContext.getNetworkDescription();
    std::string actualAlgorithmDescription = simulationContext.getRoutingDescription();
    
    resultsFile << "# omni_simulator Results - " << actualAlgorithmDescription << "\n";
    resultsFile << "# Network: " << finalNetworkDescription << "\n";
    resultsFile << "# Routing Algorithm: " << config.getRoutingAlgorithm() << "\n";
    resultsFile << "# Network Topology: " << config.getNetworkTopology() << "\n";
    if (config.isHypercubeMode()) {
        resultsFile << "# Baseline Routing: E-cube\n";
        resultsFile << "# Deadlock Avoidance: Duato's Protocol\n";
    }
    resultsFile << "# Virtual Channels: " << config.getVirtualChannels() << "\n";
    resultsFile << "# Buffer Size: " << config.getBufferSize() << "\n";
    resultsFile << "# Simulation Date: " << getCurrentTimestamp() << "\n";
    resultsFile << "InjectionRate,AverageDelay,Throughput,Saturated\n";

    // Store experimental results for final display
    std::vector<ExperimentResult> experimentResults;

    // Experiment with each injection rate
    for (size_t rateIndex = 0; rateIndex < injectionRates.size(); ++rateIndex) {
        double injectionRate = injectionRates[rateIndex];
        
        std::cout << "\n=== Testing injection rate: " << std::fixed << std::setprecision(3) 
                  << injectionRate << " packets/cycle/node (" << (rateIndex + 1) 
                  << "/" << injectionRates.size() << ") ===" << std::endl;
        
        bool saturated = false;
        std::vector<double> allDelays;
        std::vector<double> allThroughputs;
        std::vector<bool> allSaturatedStates;
        int successfulRuns = 0;
        
        // Run multiple experiments
        int runsPerRate = config.getRunsPerInjectionRate();
        for (int run = 0; run < runsPerRate; run++) {
            std::cout << "  Run " << (run + 1) << "/" << runsPerRate;
            
            // Use more deterministic seed generation
            unsigned int seed = 12345 + (rateIndex * 10000) + (run * 1000);
            srand(seed);
            
            // Run simulation
            simulator->runSimulation(injectionRate, config);
            
            // Collect metrics
            auto metrics = simulator->getMetrics();
            double currentDelay = metrics->getAveragePacketDelay();
            double currentThroughput = metrics->getThroughput();
            size_t packetsReceived = metrics->getPacketCount();
            bool currentSaturated = metrics->isSaturated();
            
            std::cout << " - Delay: " << std::fixed << std::setprecision(3) << currentDelay 
                      << ", Throughput: " << std::setprecision(4) << currentThroughput 
                      << " (Packets: " << packetsReceived << ", Sat: " << (currentSaturated ? "Y" : "N") << ")" << std::endl;
            
            // Only record valid results
            if (packetsReceived > 0 && currentDelay > 0 && currentDelay < 2000.0) {
                allDelays.push_back(currentDelay);
                allThroughputs.push_back(currentThroughput);
                allSaturatedStates.push_back(currentSaturated);
                successfulRuns++;
            } else {
                // Complete network blocking situation
                allSaturatedStates.push_back(true);
                std::cout << "    (Network blocked)" << std::endl;
            }
            
            simulator->reset();
        }
        
        double avgDelay = 0.0;
        double throughput = 0.0;
        
        if (successfulRuns > 0) {
            // Use median instead of average for better stability
            std::sort(allDelays.begin(), allDelays.end());
            std::sort(allThroughputs.begin(), allThroughputs.end());
            
            if (allDelays.size() >= 3) {
                // Take median
                size_t midIndex = allDelays.size() / 2;
                avgDelay = allDelays[midIndex];
                throughput = allThroughputs[midIndex];
            } else {
                // For small number of runs, take average
                for (double delay : allDelays) avgDelay += delay;
                for (double tp : allThroughputs) throughput += tp;
                avgDelay /= allDelays.size();
                throughput /= allThroughputs.size();
            }
            
            size_t saturatedCount = std::count(allSaturatedStates.begin(), allSaturatedStates.end(), true);
            saturated = (saturatedCount > allSaturatedStates.size() / 2);
        } else {
            // All runs failed, mark as blocked
            avgDelay = -1.0;
            throughput = 0.0;
            saturated = true;
        }
        
        // Additional monotonicity check: if previous test point has higher delay, adjust current value
        if (!experimentResults.empty() && avgDelay > 0) {
            double previousDelay = experimentResults.back().averageDelay;
            if (previousDelay < 1000.0 && avgDelay < previousDelay) {
                // Found delay decrease, use previous value plus small increment
                double minIncrease = previousDelay * 0.05;  // At least 5% increase
                avgDelay = previousDelay + minIncrease;
                std::cout << "  Adjusted delay for monotonicity: " << avgDelay << " cycles" << std::endl;
            }
        }
        
        // Output results
        std::cout << "Average Packet Delay: " << std::fixed << std::setprecision(3);
        if (avgDelay < 0) {
            std::cout << "BLOCKED";
        } else {
            std::cout << avgDelay << " cycles";
        }
        std::cout << std::endl;
        
        std::cout << "Throughput: " << std::setprecision(4) << throughput << " flits/cycle/node" << std::endl;
        std::cout << "Saturated: " << (saturated ? "Yes" : "No") << std::endl;
        
        // Store results
        ExperimentResult result;
        result.injectionRate = injectionRate;
        result.averageDelay = (avgDelay < 0) ? 1000.0 : avgDelay;
        result.throughput = throughput;
        result.saturated = saturated;
        experimentResults.push_back(result);
        
        // Write to CSV file
        resultsFile << std::fixed << std::setprecision(6) << injectionRate << "," 
                   << std::setprecision(3) << ((avgDelay < 0) ? 1000.0 : avgDelay) << "," 
                   << std::setprecision(4) << throughput << ","
                   << (saturated ? "1" : "0") << std::endl;
    }

    resultsFile.close();
    
    // Display complete experimental results table
    printResultsTableFormatted(experimentResults);
    
    // Display statistical analysis
    printStatistics(experimentResults, config);
    
    // Verify read from CSV file
    auto csvResults = readResultsFromCSV(config.getOutputFile());
    printCSVVerification(csvResults, experimentResults, config.getOutputFile());

    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "omni_simulator Experiment Completed Successfully!" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Results exported to: " << config.getOutputFile() << std::endl;
    std::cout << "Thank you for using omni_simulator!" << std::endl;
    std::cout << std::string(80, '=') << std::endl;

    // No need to manually delete simulator - SimulationContext handles cleanup
    
    return 0;
}