# omni_simulator Project

[![CI - Experiment Test](https://github.com/nash635/omni_simulator/actions/workflows/experiment-test.yml/badge.svg)](https://github.com/nash635/omni_simulator/actions/workflows/experiment-test.yml)
[![CI/CD Pipeline](https://github.com/nash635/omni_simulator/actions/workflows/ci.yml/badge.svg)](https://github.com/nash635/omni_simulator/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)


### Quick CI Test (Local)

Test the CI setup locally before pushing:

```bash
# Test CI configuration and experiment
chmod +x test_ci_setup.sh
./test_ci_setup.sh

# Deploy CI/CD setup to GitHub
chmod +x deploy_ci.sh
./deploy_ci.sh
```

## Overview

omni_simulator is a comprehensive network routing simulator designed to model and analyze routing protocols in interconnection networks. The simulator implements advanced deadlock-free routing algorithms including Duato's protocol and supports multiple network topologies such as 2D mesh and hypercube networks.

## System Requirements

### Operating System
- **Ubuntu 20.04** or higher
- **CentOS 8** or equivalent Linux distributions
- **macOS 15.5** or higher (experimental support)

### Compiler Requirements
- **GCC**: Version 6.4.0 or higher
- **C++ Standard**: C++14 or higher
- **Clang**: Version 7.0 or higher (alternative to GCC)

### Build System
- **Bazel**: Version 0.24.1 or higher
- **Python**: Version 3.6 or higher (for Bazel)

### Hardware Requirements
- **Memory**: Minimum 4GB RAM (8GB recommended)
- **Storage**: At least 1GB free disk space
- **CPU**: Multi-core processor recommended for parallel compilation

## Project Structure

The project follows a modular architecture organized into several directories:

```
omni_simulator/
├── BUILD                          # Bazel build configuration
├── WORKSPACE                      # Bazel workspace configuration
├── LICENCE                        # MIT license file
├── README.md                      # This documentation
├── config.json                    # Default simulation configuration
├── run_experiment.sh              # Automated experiment runner script
├── exp/                          # Experimental configurations
└── src/                          # Source code directory
    ├── main.cpp                  # Entry point and experiment runner
    ├── message/                  # Message and packet definitions
    ├── metrics/                  # Performance metrics collection
    ├── network/                  # Network topology implementations
    ├── routing/                  # Routing algorithm implementations
    ├── simulator/                # Core simulation engine
    ├── traffic/                  # Traffic pattern generators
    └── utils/                    # Utility classes and configurations
```

## Features

### Network Topologies
- **2D Mesh Networks**: Configurable grid topologies with torus support
- **Hypercube Networks**: Multi-dimensional hypercube topologies (up to 4D)
- **Virtual Channels**: Multiple virtual channels per physical link

### Routing Algorithms
- **Duato's Protocol**: Deadlock-free adaptive routing for 2D mesh
- **E-cube Routing**: Dimension-ordered routing for hypercube networks
- **Duato's Hypercube Protocol**: Deadlock-free routing for hypercube topologies

### Traffic Patterns
- **Uniform Random**: Packets sent to random destinations
- **Transpose**: Structured communication patterns
- **Hotspot**: Concentrated traffic to specific nodes
- **Hypercube Uniform**: Optimized uniform traffic for hypercube networks

### Performance Metrics
- **Average Packet Latency**: End-to-end delay measurement
- **Network Throughput**: Effective bandwidth utilization
- **Saturation Detection**: Automatic detection of network congestion
- **Hop Count Analysis**: Path length statistics
- **Buffer Utilization**: Memory usage analysis

## Installation Guide

### Step 1: Install System Dependencies

#### Ubuntu/Debian:
```bash
# Update package lists
sudo apt update

# Install GCC and build essentials
sudo apt install build-essential gcc g++ -y

# Install Python and pip
sudo apt install python3 python3-pip -y

# Install Git (if not already installed)
sudo apt install git -y

# Verify GCC version (should be >= 6.4.0)
gcc --version
```

#### CentOS/RHEL:
```bash
# Install development tools
sudo yum groupinstall "Development Tools" -y
sudo yum install gcc gcc-c++ python3 python3-pip git -y

# For CentOS 8+ use dnf instead of yum
sudo dnf groupinstall "Development Tools" -y
sudo dnf install gcc gcc-c++ python3 python3-pip git -y
```

### Step 2: Install Bazel

#### Using Bazelisk (Recommended)
```bash
# Download and install Bazelisk
wget https://github.com/bazelbuild/bazelisk/releases/latest/download/bazelisk-linux-amd64
chmod +x bazelisk-linux-amd64
sudo mv bazelisk-linux-amd64 /usr/local/bin/bazel

# Verify installation
bazel version
```

### Step 3: Clone and Setup Project

```bash
# Clone the repository
git clone <repository-url>
cd omni_simulator

# Verify project structure
ls -la

# Make experiment script executable
chmod +x run_experiment.sh

# Check configuration file
cat config.json
```

## Build Instructions

### Quick Build (All Targets)
```bash
# Build entire project
bazel build //...

# Check build status
echo "Build completed with exit code: $?"
```

### Incremental Build (Specific Targets)
```bash
# Build only the main simulator binary
bazel build //:simulator_binary

# Build specific libraries
bazel build //:network
bazel build //:routing
bazel build //:simulator
```

### Debug Build
```bash
# Build with debug information
bazel build //... --compilation_mode=dbg

# Build with optimizations
bazel build //... --compilation_mode=opt
```

### Clean Build
```bash
# Clean all build artifacts
bazel clean

# Deep clean (removes all cached data)
bazel clean --async
```

## Running the Simulator

### Quick Start with Experiment Runner Script

The easiest way to run experiments is using the provided `run_experiment.sh` script:

```bash
# Make script executable (first time only)
chmod +x run_experiment.sh

# List available experiments
./run_experiment.sh --list

# Run a specific experiment
./run_experiment.sh duato_2d_mesh
./run_experiment.sh duato_on_ecube_hypercube

# Show help and usage options
./run_experiment.sh --help
```

### Experiment Runner Script Features

#### Basic Usage
```bash
# Run single experiment
./run_experiment.sh <experiment_name>

# Examples:
./run_experiment.sh duato_2d_mesh                    # Run 2D mesh experiment
./run_experiment.sh duato_on_ecube_hypercube         # Run hypercube experiment
```

#### Available Experiment Configurations

The script automatically discovers experiments in the `exp/` directory:

1. **duato_2d_mesh**: Duato's protocol on 2D mesh topology
2. **duato_on_ecube_hypercube**: Duato's protocol on hypercube with E-cube baseline

Each experiment includes:
- Network topology configuration
- Routing algorithm settings
- Traffic pattern specifications
- Performance measurement parameters

#### Automated Features

The experiment runner provides:

- **System Requirements Check**: Verifies GCC and Bazel versions
- **Automatic Build Management**: Handles compilation with various options
- **Configuration Management**: Backs up and restores config files automatically
- **Results Organization**: Saves results with timestamps in organized directories
- **Batch Processing**: Runs multiple experiments sequentially
- **Error Handling**: Graceful failure recovery and cleanup
- **Progress Reporting**: Colored output with clear status indicators

### CI/CD Integration

The repository includes comprehensive CI/CD integration that automatically validates experiment execution:

#### Automated Testing
Every push and pull request triggers automated testing that:
- ✅ **Builds the project** with multiple GCC versions (9, 11)
- ✅ **Validates system requirements** (GCC, Bazel, Python)
- ✅ **Runs the core experiment**: `./run_experiment.sh duato_on_ecube_hypercube`
- ✅ **Checks result generation** and output validation
- ✅ **Tests cross-platform compatibility** (Ubuntu 20.04/22.04)

#### CI Status Monitoring
Monitor the health of the project through these automated checks:

| Check Type | Current Status | Description |
|------------|----------------|-------------|
| **Experiment Test** | ![Experiment](https://github.com/nash635/omni_simulator/actions/workflows/experiment-test.yml/badge.svg) | Core experiment execution |
| **Full CI Pipeline** | ![CI](https://github.com/nash635/omni_simulator/actions/workflows/ci.yml/badge.svg) | Complete build and test suite |

#### Viewing Test Results
- **Live Status**: Check the [Actions tab](https://github.com/nash635/omni_simulator/actions) for real-time CI results
- **Test Artifacts**: Download experiment outputs and logs from completed CI runs
- **Failure Investigation**: Detailed logs available for debugging failed experiments

#### Local CI Validation
You can run the same validations locally:

```bash
# Run the CI validation script
chmod +x .github/scripts/validate_ci.sh
.github/scripts/validate_ci.sh

# Run the experiment test script
chmod +x .github/scripts/run_experiment_test.sh
.github/scripts/run_experiment_test.sh
```

### Manual Execution (Alternative Method)

If you prefer manual control, you can also run experiments directly:

```bash
# Build the project first
bazel build //...

# Copy experiment configuration
cp exp/duato_2d_mesh/duato_2d_mesh.json config.json

# Run the simulation
bazel run //:simulator_binary

# Or run the built binary directly
./bazel-bin/simulator_binary
```

### Configuration Options

#### Available Experiment Types

1. **2D Mesh with Duato's Protocol**
   ```bash
   ./run_experiment.sh duato_2d_mesh
   ```

2. **Hypercube with Duato's Protocol**
   ```bash
   ./run_experiment.sh duato_on_ecube_hypercube
   ```

#### Custom Configuration
You can also modify configurations directly:

```bash
# Edit experiment configuration
vim exp/duato_2d_mesh/duato_2d_mesh.json

# Run with modified configuration
./run_experiment.sh duato_2d_mesh
```


### Performance Optimization

#### Build Optimizations
```bash
# Use optimized build for better performance
./run_experiment.sh -o duato_2d_mesh

# Clean build for consistent results
./run_experiment.sh -c duato_2d_mesh
```

#### Batch Processing
```bash
# Run all experiments overnight
nohup ./run_experiment.sh --batch > batch_results.log 2>&1 &

# Monitor progress
tail -f batch_results.log
```

## Experimental Results
Experiment report for [duato_on_ecube_hypercube](exp/duato_on_ecube_hypercube/duato_on_ecube_hypercube.ipynb)

## Citation

If you use omni_simulator in your research, please cite:

```bibtex
@software{omni_simulator2025,
  title={omni_simulator: Network Routing Simulator},
  author={nash635},
  email={shaj24@mails.tsinghua.edu.cn},
  year={2025},
  url={https://github.com/nash635/omni_simulator},
  license={MIT}
}
```

## Contributing

We welcome contributions to omni_simulator! Please follow these guidelines:

1. **Fork** the repository
2. **Create** a feature branch
3. **Implement** your changes with appropriate tests
4. **Submit** a pull request with detailed description

## Future Work

The project is designed with extensibility in mind. Planned enhancements include:

- **Additional Topologies**: 3D mesh, torus, and fat-tree networks
- **Advanced Routing**: Adaptive routing algorithms and machine learning-based routing
- **Traffic Patterns**: Real-world application traces and synthetic patterns
- **Performance Tools**: GUI interface and real-time visualization
- **Scalability**: Support for larger networks and distributed simulation

---

**omni_simulator** - Advanced Network Routing Simulation Framework  
**Copyright (c) 2025 nash635. All rights reserved.**