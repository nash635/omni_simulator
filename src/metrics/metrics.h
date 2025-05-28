/*
 * omni_simulator - Network Routing Simulator
 * Performance Metrics Collection and Analysis
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef METRICS_H
#define METRICS_H

#include <vector>
#include <string>
#include <fstream>
#include <utility>

class Metrics {
public:
    Metrics();
    ~Metrics();
    
    // Record metrics
    void recordPacketLatency(double latencyCycles);
    void recordFlitThroughput(double flitsPerCyclePerNode);
    void recordInjectionRate(double packetsPerCyclePerNode);
    void recordLatency(double latency);
    void recordThroughput(double throughput);
    void recordHopCount(int hops);
    void recordNetworkUtilization(double utilization);
    void recordCongestionEvent();

    // Calculate metrics
    double getAveragePacketDelay() const;
    double getThroughput() const;
    double getCurrentInjectionRate() const;
    double calculateAverageLatency() const;
    double calculateThroughput() const;
    double getAverageLatency() const;
    double getAverageHopCount() const;
    double getAverageNetworkUtilization() const;
    int getCongestionEvents() const;

    // Experiment control
    void startWarmup();
    void startMeasurement();
    void endMeasurement();
    void reset();
    
    // Saturation detection
    bool isSaturated() const;
    bool isSaturated(double latencyThreshold, double throughputDropThreshold) const;
    void setSaturated(bool saturated);
    
    void exportToCSV(const std::string& filename) const;
    void printCurrentMetrics() const;
    void printFinalResults() const;
    void printMetrics() const;

    const std::vector<double>& getPacketLatencies() const;
    size_t getPacketCount() const;

private:
    double totalLatency;
    double totalThroughput;
    int count;
    int totalPackets;
    double totalFlits;
    int measurementCycles;
    double currentInjectionRate;
    bool isWarmupPhase;
    bool isMeasurementPhase;
    int currentCycle;
    bool saturated;
    
    std::vector<double> packetLatencies;
    std::vector<double> latencies;
    std::vector<double> flitThroughputs;
    std::vector<double> throughputs;
    std::vector<double> injectionRates;
    std::vector<double> throughputSamples;
    std::vector<double> networkUtilizations;

    std::vector<std::pair<double, double>> historyLatencyThroughput;

    std::vector<int> hopCounts;
    int totalHops;
    int congestionEvents;
};

#endif // METRICS_H