#include "metrics.h"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <fstream>

Metrics::Metrics() : totalLatency(0.0), totalThroughput(0.0), count(0), totalPackets(0), totalFlits(0.0), 
                     measurementCycles(0), currentInjectionRate(0.0), isWarmupPhase(false), 
                     isMeasurementPhase(false), currentCycle(0), saturated(false), totalHops(0), 
                     congestionEvents(0) {
}

Metrics::~Metrics() {
}

void Metrics::recordLatency(double latency) {
    latencies.push_back(latency);
    totalLatency += latency;
    count++;
}

void Metrics::recordThroughput(double throughput) {
    throughputs.push_back(throughput);
    throughputSamples.push_back(throughput);
    totalThroughput += throughput;
}

void Metrics::recordPacketLatency(double latencyCycles) {
    packetLatencies.push_back(latencyCycles);
    recordLatency(latencyCycles);
}

void Metrics::recordFlitThroughput(double flitsPerCyclePerNode) {
    flitThroughputs.push_back(flitsPerCyclePerNode);
    throughputSamples.push_back(flitsPerCyclePerNode);
    totalFlits += flitsPerCyclePerNode;
    recordThroughput(flitsPerCyclePerNode);
}

void Metrics::recordInjectionRate(double packetsPerCyclePerNode) {
    injectionRates.push_back(packetsPerCyclePerNode);
    currentInjectionRate = packetsPerCyclePerNode;
}

void Metrics::recordHopCount(int hops) {
    hopCounts.push_back(hops);
    totalHops += hops;
}

double Metrics::calculateAverageLatency() const {
    return count > 0 ? totalLatency / count : 0.0;
}

double Metrics::calculateThroughput() const {
    if (throughputs.empty()) {
        return 0.0;
    }
    return totalThroughput / throughputs.size();
}

double Metrics::getAverageLatency() const {
    return calculateAverageLatency();
}

double Metrics::getThroughput() const {
    if (!flitThroughputs.empty()) {
        double sum = 0.0;
        for (double throughput : flitThroughputs) {
            sum += throughput;
        }
        return sum / flitThroughputs.size();
    }
    
    return calculateThroughput();
}

double Metrics::getAveragePacketDelay() const {
    if (packetLatencies.empty()) {
        if (isMeasurementPhase) {
            return -1.0;
        }
        
        if (!latencies.empty()) {
            return getAverageLatency();
        }
        return 0.0;
    }
    
    double sum = 0.0;
    for (double latency : packetLatencies) {
        sum += latency;
    }
    return sum / packetLatencies.size();
}

double Metrics::getCurrentInjectionRate() const {
    return currentInjectionRate;
}

double Metrics::getAverageHopCount() const {
    if (hopCounts.empty()) return 0.0;
    return static_cast<double>(totalHops) / hopCounts.size();
}

void Metrics::startWarmup() {
    isWarmupPhase = true;
    isMeasurementPhase = false;
    currentCycle = 0;
}

void Metrics::startMeasurement() {
    isMeasurementPhase = true;
    isWarmupPhase = false;
    packetLatencies.clear();
    flitThroughputs.clear();
    throughputSamples.clear();
}

void Metrics::endMeasurement() {
    isMeasurementPhase = false;
}

bool Metrics::isSaturated() const {
    double defaultLatencyThreshold = 50.0;
    double defaultThroughputDropThreshold = 0.1;
    
    return isSaturated(defaultLatencyThreshold, defaultThroughputDropThreshold);
}

bool Metrics::isSaturated(double latencyThreshold, double throughputDropThreshold) const {
    if (saturated) {
        return true;
    }
    
    double avgLatency = getAverageLatency();
    if (avgLatency > latencyThreshold) {
        return true;
    }
    
    if (throughputSamples.size() < 2) {
        return false;
    }
    
    size_t n = throughputSamples.size();
    double currentThroughput = throughputSamples[n-1];
    double previousThroughput = throughputSamples[n-2];
    
    if (currentThroughput == 0.0) {
        return true;
    }
    
    if (previousThroughput > 0 && 
        (previousThroughput - currentThroughput) / previousThroughput > throughputDropThreshold) {
        return true;
    }
    
    return false;
}

void Metrics::setSaturated(bool saturated) {
    this->saturated = saturated;
}

void Metrics::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    file << "Cycle,PacketLatency,FlitThroughput,InjectionRate\n";
    
    size_t maxSize = std::max({packetLatencies.size(), flitThroughputs.size(), injectionRates.size()});
    for (size_t i = 0; i < maxSize; ++i) {
        file << i;
        
        if (i < packetLatencies.size()) {
            file << "," << packetLatencies[i];
        } else {
            file << ",";
        }
        
        if (i < flitThroughputs.size()) {
            file << "," << flitThroughputs[i];
        } else {
            file << ",";
        }
        
        if (i < injectionRates.size()) {
            file << "," << injectionRates[i];
        } else {
            file << ",";
        }
        
        file << "\n";
    }
    file.close();
}

void Metrics::printCurrentMetrics() const {
    std::cout << "Current Metrics:" << std::endl;
    std::cout << "  Average Packet Delay: " << getAveragePacketDelay() << " cycles" << std::endl;
    std::cout << "  Throughput: " << getThroughput() << " flits/cycle/node" << std::endl;
    std::cout << "  Current Injection Rate: " << getCurrentInjectionRate() << " packets/cycle/node" << std::endl;
}

void Metrics::printFinalResults() const {
    std::cout << "\n=== Final Simulation Results ===" << std::endl;
    std::cout << "Average Packet Delay: " << getAveragePacketDelay() << " cycles" << std::endl;
    std::cout << "Average Throughput: " << getThroughput() << " flits/cycle/node" << std::endl;
    std::cout << "Total Packets: " << packetLatencies.size() << std::endl;
    std::cout << "Measurement Cycles: " << measurementCycles << std::endl;
    
    if (packetLatencies.size() > 0) {
        double minLatency = *std::min_element(packetLatencies.begin(), packetLatencies.end());
        double maxLatency = *std::max_element(packetLatencies.begin(), packetLatencies.end());
        std::cout << "Latency Range: " << minLatency << " - " << maxLatency << " cycles" << std::endl;
    }
}

void Metrics::printMetrics() const {
    std::cout << "Average Latency: " << getAverageLatency() << std::endl;
    std::cout << "Total Throughput: " << getThroughput() << std::endl;
}

void Metrics::reset() {
    latencies.clear();
    throughputs.clear();
    packetLatencies.clear();
    flitThroughputs.clear();
    injectionRates.clear();
    historyLatencyThroughput.clear();
    throughputSamples.clear();
    hopCounts.clear();
    networkUtilizations.clear();
    
    totalLatency = 0.0;
    totalThroughput = 0.0;
    totalFlits = 0.0;
    totalPackets = 0;
    totalHops = 0;
    count = 0;
    measurementCycles = 0;
    currentInjectionRate = 0.0;
    currentCycle = 0;
    congestionEvents = 0;
    
    isWarmupPhase = false;
    isMeasurementPhase = false;
    saturated = false;
}

int Metrics::getCongestionEvents() const {
    return congestionEvents;
}

void Metrics::recordCongestionEvent() {
    congestionEvents++;
}

void Metrics::recordNetworkUtilization(double utilization) {
    networkUtilizations.push_back(utilization);
}

double Metrics::getAverageNetworkUtilization() const {
    if (networkUtilizations.empty()) return 0.0;
    double sum = 0.0;
    for (double util : networkUtilizations) {
        sum += util;
    }
    return sum / networkUtilizations.size();
}

const std::vector<double>& Metrics::getPacketLatencies() const {
    return packetLatencies;
}

size_t Metrics::getPacketCount() const {
    return packetLatencies.size();
}