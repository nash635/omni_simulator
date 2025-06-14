#!/bin/bash

# omni_simulator Experiment Runner
# Copyright (c) 2025 nash635

set -e  # Exit on any error

# Enable debug mode for troubleshooting
DEBUG_MODE=${DEBUG_MODE:-false}
if [ "$DEBUG_MODE" = "true" ]; then
    set -x  # Print commands as they execute
fi

# Color codes for output formatting
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"
EXP_DIR="$PROJECT_ROOT/exp"
CONFIG_BACKUP="$PROJECT_ROOT/config.json.backup"
RESULTS_DIR="$PROJECT_ROOT/results"
BUILD_LOG="$PROJECT_ROOT/build.log"
RUN_LOG="$PROJECT_ROOT/run.log"

# Function to print colored output
print_header() {
    echo -e "${BLUE}================================================================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}================================================================================${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_info() {
    echo -e "${CYAN}ℹ $1${NC}"
}

print_debug() {
    if [ "$DEBUG_MODE" = "true" ]; then
        echo -e "${PURPLE}[DEBUG] $1${NC}"
    fi
}

# Function to show usage
show_usage() {
    echo -e "${PURPLE}omni_simulator Experiment Runner${NC}"
    echo ""
    echo "Usage: $0 [OPTIONS] [EXPERIMENT_CONFIG]"
    echo ""
    echo "Arguments:"
    echo "  EXPERIMENT_CONFIG    Path to experiment configuration file"
    echo "                      (relative to exp/ directory or absolute path)"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -l, --list          List available experiment configurations"
    echo "  -c, --clean         Clean build before compiling"
    echo "  -o, --optimize      Build with optimizations"
    echo "  -d, --debug         Build with debug information"
    echo "  -r, --results-dir   Specify custom results directory"
    echo "  -b, --batch         Run all available experiments"
    echo "  -v, --verbose       Enable verbose output"
    echo "  -q, --quiet         Suppress simulation output (only show logs)"
    echo "  --no-build          Skip build step (use existing binary)"
    echo "  --build-only        Only build, don't run simulation"
    echo "  --debug-mode        Enable debug mode (shows all commands)"
    echo ""
    echo "Examples:"
    echo "  $0 duato_2d_mesh                    # Run duato_2d_mesh experiment"
    echo "  $0 duato_on_ecube_hypercube         # Run hypercube experiment"
    echo "  $0 -c duato_2d_mesh                 # Clean build then run"
    echo "  $0 -v duato_on_ecube_hypercube      # Run with verbose output"
    echo "  $0 --debug-mode duato_2d_mesh       # Run with debug mode"
    echo "  $0 -l                               # List available experiments"
    echo ""
    echo "Available experiment configurations:"
    if [ -d "$EXP_DIR" ]; then
        for exp_dir in "$EXP_DIR"/*; do
            if [ -d "$exp_dir" ]; then
                exp_name=$(basename "$exp_dir")
                config_file="$exp_dir/$exp_name.json"
                if [ -f "$config_file" ]; then
                    echo -e "  ${GREEN}$exp_name${NC}"
                    print_debug "  Config file: $config_file"
                else
                    echo -e "  ${YELLOW}$exp_name${NC} (config file missing)"
                    print_debug "  Expected: $config_file"
                fi
            fi
        done
    else
        echo "  No experiment directory found at $EXP_DIR"
    fi
}

# Function to list available experiments
list_experiments() {
    print_header "Available Experiment Configurations"
    
    if [ ! -d "$EXP_DIR" ]; then
        print_error "Experiment directory not found: $EXP_DIR"
        return 1
    fi
    
    print_debug "Scanning directory: $EXP_DIR"
    
    local count=0
    for exp_dir in "$EXP_DIR"/*; do
        if [ -d "$exp_dir" ]; then
            exp_name=$(basename "$exp_dir")
            config_file="$exp_dir/$exp_name.json"
            
            print_debug "Checking experiment: $exp_name"
            print_debug "Expected config: $config_file"
            
            if [ -f "$config_file" ]; then
                count=$((count + 1))
                echo -e "${count}. ${GREEN}$exp_name${NC}"
                
                # Extract some key information from the config file
                if command -v jq >/dev/null 2>&1; then
                    description=$(jq -r '.simulation.description // "No description"' "$config_file" 2>/dev/null)
                    topology=$(jq -r '.network.topology // "Unknown"' "$config_file" 2>/dev/null)
                    algorithm=$(jq -r '.routing.algorithm // "Unknown"' "$config_file" 2>/dev/null)
                else
                    description=$(grep -o '"description"[[:space:]]*:[[:space:]]*"[^"]*"' "$config_file" | cut -d'"' -f4 | head -1 2>/dev/null || echo "No description")
                    topology=$(grep -o '"topology"[[:space:]]*:[[:space:]]*"[^"]*"' "$config_file" | cut -d'"' -f4 | head -1 2>/dev/null || echo "Unknown")
                    algorithm=$(grep -o '"algorithm"[[:space:]]*:[[:space:]]*"[^"]*"' "$config_file" | cut -d'"' -f4 | head -1 2>/dev/null || echo "Unknown")
                fi
                
                echo "   Description: ${description:-"No description"}"
                echo "   Topology: ${topology:-"Unknown"}"
                echo "   Algorithm: ${algorithm:-"Unknown"}"
                echo "   Config: $config_file"
                echo ""
            else
                echo -e "${count}. ${YELLOW}$exp_name${NC} (config file missing: $config_file)"
            fi
        fi
    done
    
    if [ $count -eq 0 ]; then
        print_warning "No valid experiment configurations found"
        print_info "Directory contents:"
        ls -la "$EXP_DIR" 2>/dev/null || echo "  Directory not accessible"
        return 1
    fi
    
    print_info "Found $count experiment configuration(s)"
    return 0
}

# 简化的系统要求检查函数
check_requirements() {
    print_info "Checking system requirements..."
    
    local requirements_met=true
    
    # 简化的GCC检查
    if command -v gcc >/dev/null 2>&1; then
        gcc_version=$(gcc --version | head -n1 | awk '{print $NF}' 2>/dev/null || echo "unknown")
        print_success "GCC found: $gcc_version"
        
        # 简化版本检查 - 只要能找到GCC就认为满足要求
        if [[ "$gcc_version" =~ ^[0-9]+\.[0-9]+ ]]; then
            print_success "GCC version acceptable"
        else
            print_warning "GCC version format unknown, but continuing"
        fi
    else
        print_error "GCC not found. Please install GCC"
        requirements_met=false
    fi
    
    # 简化的Bazel检查
    if command -v bazel >/dev/null 2>&1; then
        print_success "Bazel found"
        # 简单测试bazel是否能运行
        if bazel version >/dev/null 2>&1; then
            print_success "Bazel is functional"
        else
            print_warning "Bazel found but may have issues"
        fi
    else
        print_error "Bazel not found. Please install Bazel"
        requirements_met=false
    fi
    
    # 检查项目结构
    if [ ! -f "$PROJECT_ROOT/BUILD" ] || [ ! -f "$PROJECT_ROOT/WORKSPACE" ]; then
        print_error "Missing BUILD or WORKSPACE files"
        requirements_met=false
    else
        print_success "Project structure verified"
    fi
    
    return $([ "$requirements_met" = "true" ] && echo 0 || echo 1)
}

setup_experiment_config() {
    local experiment_name="$1"
    local results_file="$2"
    
    print_info "Setting up experiment: $experiment_name"
    
    # 更健壮的配置文件查找
    local config_file=""
    local search_paths=(
        "$EXP_DIR/$experiment_name/$experiment_name.json"
        "$EXP_DIR/$experiment_name.json"
        "$experiment_name"
    )
    
    for path in "${search_paths[@]}"; do
        if [ -f "$path" ] && [ -r "$path" ]; then
            config_file="$path"
            print_success "Found config: $config_file"
            break
        fi
    done
    
    if [ -z "$config_file" ]; then
        print_error "Configuration file not found for: $experiment_name"
        print_info "Searched paths:"
        for path in "${search_paths[@]}"; do
            echo "  - $path"
        done
        return 1
    fi
    
    # 验证JSON格式
    if command -v python3 >/dev/null 2>&1; then
        if ! python3 -m json.tool "$config_file" >/dev/null 2>&1; then
            print_error "Invalid JSON format in: $config_file"
            return 1
        fi
    fi
    
    # 备份和复制配置
    if [ -f "$PROJECT_ROOT/config.json" ]; then
        cp "$PROJECT_ROOT/config.json" "$PROJECT_ROOT/config.json.backup"
    fi
    
    cp "$config_file" "$PROJECT_ROOT/config.json"
    print_success "Configuration copied successfully"
    
    return 0
}

# Function to setup experiment config
setup_experiment_config() {
    local experiment_name="$1"
    local results_file="$2"
    local config_file=""
    
    print_debug "Setting up experiment: $experiment_name"
    print_debug "Results file: $results_file"
    
    # 查找配置文件
    local search_paths=(
        "$EXP_DIR/$experiment_name/$experiment_name.json"
        "$EXP_DIR/$experiment_name.json"
        "$experiment_name"
    )
    
    for path in "${search_paths[@]}"; do
        if [ -f "$path" ] && [ -r "$path" ]; then
            config_file="$path"
            print_success "Found config: $config_file"
            break
        fi
    done
    
    if [ -z "$config_file" ]; then
        print_error "Configuration file not found for: $experiment_name"
        print_info "Searched paths:"
        for path in "${search_paths[@]}"; do
            echo "  - $path"
        done
        return 1
    fi
    
    # 验证JSON格式
    if command -v python3 >/dev/null 2>&1; then
        if ! python3 -m json.tool "$config_file" >/dev/null 2>&1; then
            print_error "Invalid JSON format in: $config_file"
            return 1
        fi
    fi
    
    # 备份和复制配置
    if [ -f "$PROJECT_ROOT/config.json" ]; then
        cp "$PROJECT_ROOT/config.json" "$PROJECT_ROOT/config.json.backup"
    fi
    
    cp "$config_file" "$PROJECT_ROOT/config.json"
    
    # 更新配置文件中的输出路径 - 使用实验目录的结果路径
    if [ -n "$results_file" ]; then
        print_info "Updating output file path to: $results_file"
        
        if command -v python3 >/dev/null 2>&1; then
            print_debug "Using Python3 to update config..."
            python3 -c "
import json
import sys
import os

config_path = '$PROJECT_ROOT/config.json'
results_path = '$results_file'

try:
    with open(config_path, 'r') as f:
        config = json.load(f)
    
    # 更新输出文件路径 - 同时设置多个可能的字段
    if 'metrics' not in config:
        config['metrics'] = {}
    
    config['metrics']['output_file'] = results_path
    config['output_file'] = results_path
    
    # 如果存在其他输出文件相关字段也一并更新
    if 'simulation' in config:
        config['simulation']['output_file'] = results_path
    
    with open(config_path, 'w') as f:
        json.dump(config, f, indent=2)
    
    print('Successfully updated output_file paths in config.json')
    
except Exception as e:
    print(f'Error updating config.json: {e}', file=sys.stderr)
    sys.exit(1)
" && print_success "Configuration updated successfully"
        else
            print_warning "Python3 not available, output file path not updated"
        fi
        
        # 验证更新
        if grep -q "$(basename "$results_file")" "$PROJECT_ROOT/config.json" 2>/dev/null; then
            print_success "Verified: output_file updated in config.json"
        else
            print_warning "Warning: Could not verify config.json update"
        fi
    fi
    
    print_success "Using experiment configuration: $config_file"
    return 0
}

# Function to backup current config
backup_config() {
    if [ -f "$PROJECT_ROOT/config.json" ]; then
        cp "$PROJECT_ROOT/config.json" "$CONFIG_BACKUP"
        print_debug "Backed up current config.json"
    fi
}

# Function to restore config  
restore_config() {
    if [ -f "$CONFIG_BACKUP" ]; then
        cp "$CONFIG_BACKUP" "$PROJECT_ROOT/config.json"
        rm -f "$CONFIG_BACKUP"
        print_debug "Restored original config.json"
    fi
}

# Function to build the project
build_project() {
    local build_mode="$1"
    local clean_build="$2"
    local verbose="$3"
    
    print_header "Building omni_simulator"
    
    cd "$PROJECT_ROOT"
    
    if [ "$clean_build" = "true" ]; then
        print_info "Cleaning previous build..."
        bazel clean >"$BUILD_LOG" 2>&1
        print_success "Build cache cleaned"
    fi
    
    local build_cmd="bazel build //..."
    
    case "$build_mode" in
        "opt")
            build_cmd="$build_cmd --compilation_mode=opt"
            print_info "Building with optimizations..."
            ;;
        "dbg") 
            build_cmd="$build_cmd --compilation_mode=dbg"
            print_info "Building with debug information..."
            ;;
        *)
            print_info "Building with default settings..."
            ;;
    esac
    
    print_debug "Build command: $build_cmd"
    
    if [ "$verbose" = "true" ]; then
        eval "$build_cmd" 2>&1 | tee "$BUILD_LOG"
        local build_result=${PIPESTATUS[0]}
    else
        eval "$build_cmd" >"$BUILD_LOG" 2>&1
        local build_result=$?
    fi
    
    if [ $build_result -eq 0 ]; then
        print_success "Build completed successfully"
        return 0
    else
        print_error "Build failed. Check build log: $BUILD_LOG"
        echo "Last 10 lines of build log:"
        tail -n 10 "$BUILD_LOG"
        return 1
    fi
}

# Function to run simulation
run_simulation() {
    local experiment_name="$1"
    local verbose="$2"
    local quiet="$3"
    
    print_header "Running Simulation: $experiment_name"
    
    cd "$PROJECT_ROOT"
    
    # 创建实验特定的结果目录
    local exp_results_dir="$EXP_DIR/$experiment_name/results"
    mkdir -p "$exp_results_dir"
    
    local timestamp=$(date +"%Y%m%d_%H%M%S")
    local results_file="$exp_results_dir/${experiment_name}_${timestamp}.csv"
    local absolute_results_file="$(cd "$exp_results_dir" && pwd)/${experiment_name}_${timestamp}.csv"
    
    print_info "Starting simulation with configuration: $experiment_name"
    print_info "Results will be saved to: $absolute_results_file"
    
    # 确保仿真程序在项目根目录下运行
    local run_cmd="bazel run //:simulator_binary"
    print_debug "Run command: $run_cmd"
    print_debug "Current working directory: $(pwd)"
    
    if [ "$quiet" = "true" ]; then
        print_info "Running in quiet mode (output suppressed)"
        eval "$run_cmd" >"$RUN_LOG" 2>&1
        local run_result=$?
    elif [ "$verbose" = "true" ]; then
        print_info "============== SIMULATION OUTPUT START =============="
        eval "$run_cmd" 2>&1 | tee "$RUN_LOG"
        local run_result=${PIPESTATUS[0]}
        print_info "=============== SIMULATION OUTPUT END ==============="
    else
        print_info "============== SIMULATION OUTPUT START =============="
        eval "$run_cmd" 2>&1 | tee "$RUN_LOG"
        local run_result=${PIPESTATUS[0]}
        print_info "=============== SIMULATION OUTPUT END ==============="
    fi
    
    if [ $run_result -eq 0 ]; then
        print_success "Simulation completed successfully"
    else
        print_error "Simulation failed with exit code: $run_result"
        print_error "Check run log: $RUN_LOG"
        echo "Last 10 lines of run log:"
        tail -n 10 "$RUN_LOG" 2>/dev/null || echo "Could not read log file"
        return 1
    fi
}

# Function to cleanup on exit
cleanup() {
    local exit_code=$?
    print_debug "Cleanup called with exit code: $exit_code"
    
    if [ $exit_code -ne 0 ]; then
        print_error "Script terminated with errors (exit code: $exit_code)"
    fi
    
    restore_config
    exit $exit_code
}

# Set up cleanup trap
trap cleanup EXIT INT TERM

# Main function
main() {
    local experiment_config=""
    local clean_build="false"
    local build_mode="fastbuild"
    local verbose="false"
    local quiet="false"
    local results_dir=""
    local batch_mode="false"
    local no_build="false"
    local build_only="false"
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -l|--list)
                list_experiments
                exit $?
                ;;
            -c|--clean)
                clean_build="true"
                shift
                ;;
            -o|--optimize)
                build_mode="opt"
                shift
                ;;
            -d|--debug)
                build_mode="dbg"
                shift
                ;;
            -r|--results-dir)
                results_dir="$2"
                shift 2
                ;;
            -b|--batch)
                batch_mode="true"
                shift
                ;;
            -v|--verbose)
                verbose="true"
                shift
                ;;
            -q|--quiet)
                quiet="true"
                shift
                ;;
            --no-build)
                no_build="true"
                shift
                ;;
            --build-only)
                build_only="true"
                shift
                ;;
            --debug-mode)
                DEBUG_MODE="true"
                set -x
                shift
                ;;
            -*)
                print_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
            *)
                if [ -z "$experiment_config" ]; then
                    experiment_config="$1"
                else
                    print_error "Multiple experiment configurations specified"
                    show_usage
                    exit 1
                fi
                shift
                ;;
        esac
    done
    
    # Override results directory if specified
    if [ -n "$results_dir" ]; then
        RESULTS_DIR="$results_dir"
        mkdir -p "$RESULTS_DIR"
    fi
    
    # Show header
    print_header "omni_simulator Experiment Runner"
    print_info "Project root: $PROJECT_ROOT"
    print_info "Results directory: $RESULTS_DIR"
    print_debug "Debug mode: $DEBUG_MODE"
    echo ""
    
    # Check requirements
    if ! check_requirements; then
        print_error "System requirements not met"
        exit 1
    fi
    echo ""
    
    # Handle different execution modes
    if [ "$batch_mode" = "true" ]; then
        print_error "Batch mode not implemented in this version"
        exit 1
    elif [ -z "$experiment_config" ]; then
        print_error "No experiment configuration specified"
        echo ""
        show_usage
        exit 1
    else
        # Single experiment mode
        experiment_name=$(basename "$experiment_config" .json)
        
        local timestamp=$(date +"%Y%m%d_%H%M%S")
        local results_file="$RESULTS_DIR/${experiment_name}_${timestamp}.csv"
        
        if ! setup_experiment_config "$experiment_config" "$results_file"; then
            print_error "Failed to setup experiment configuration"
            exit 1
        fi
        
        # Build if needed
        if [ "$no_build" != "true" ] && [ "$build_only" != "true" ]; then
            if ! build_project "$build_mode" "$clean_build" "$verbose"; then
                print_error "Build failed"
                exit 1
            fi
            echo ""
        fi
        
        # Build only mode
        if [ "$build_only" = "true" ]; then
            if ! build_project "$build_mode" "$clean_build" "$verbose"; then
                exit 1
            fi
            print_success "Build completed. Skipping simulation."
            exit 0
        fi
        
        # Run simulation
        if [ "$no_build" != "true" ]; then
            if ! run_simulation "$experiment_name" "$verbose" "$quiet"; then
                print_error "Simulation failed"
                exit 1
            fi
        else
            print_info "Skipping build step as requested"
        fi
    fi
    
    print_success "All operations completed successfully!"
}

# Check if script is being sourced or executed
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi

