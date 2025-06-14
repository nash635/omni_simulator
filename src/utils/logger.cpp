/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#include "logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>

Logger::Logger() : logLevel(LogLevel::INFO) {
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level >= logLevel) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::cout << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] "
                  << getLevelString(level) << ": " << message << std::endl;
    }
}

void Logger::setLogLevel(LogLevel level) {
    logLevel = level;
}

std::string Logger::getLevelString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}