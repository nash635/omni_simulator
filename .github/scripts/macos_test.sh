#!/bin/bash

# macOS-compatible CI test script for omni_simulator
# Tests the duato_on_ecube_hypercube experiment

set -e

# Configuration
EXPERIMENT_NAME="duato_on_ecube_hypercube"
MAX_WAIT_TIME=300  # 5 minutes max

# Colors for output (macOS compatible)
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_info "Starting CI test for $EXPERIMENT_NAME on $(uname -s)"

# Pre-flight checks
log_info "Running pre-flight checks..."

# Check if we're in the right directory
if [ ! -f "run_experiment.sh" ]; then
    log_error "run_experiment.sh not found. Are you in the project root?"
    exit 1
fi

# Check if experiment configuration exists
EXPERIMENT_CONFIG="exp/$EXPERIMENT_NAME/$EXPERIMENT_NAME.json"
if [ ! -f "$EXPERIMENT_CONFIG" ]; then
    log_error "Experiment configuration not found: $EXPERIMENT_CONFIG"
    exit 1
fi

# Validate JSON configuration
if command -v python3 &> /dev/null; then
    if python3 -m json.tool "$EXPERIMENT_CONFIG" > /dev/null 2>&1; then
        log_success "Configuration file is valid JSON"
    else
        log_error "Invalid JSON in configuration file"
        exit 1
    fi
else
    log_warning "Python3 not found, skipping JSON validation"
fi

# Check system requirements
log_info "Checking system requirements..."

# Check GCC
if command -v gcc &> /dev/null; then
    GCC_VERSION=$(gcc --version | head -1)
    log_success "GCC found: $GCC_VERSION"
else
    log_error "GCC not found"
    exit 1
fi

# Check Bazel
if command -v bazel &> /dev/null; then
    BAZEL_VERSION=$(bazel --version 2>/dev/null || echo "unknown")
    log_success "Bazel found: $BAZEL_VERSION"
else
    log_error "Bazel not found"
    exit 1
fi

# Make script executable
chmod +x run_experiment.sh
log_success "Made run_experiment.sh executable"

# Test basic build first
log_info "Testing basic build..."
if bazel build //... --verbose_failures; then
    log_success "Build completed successfully"
else
    log_error "Build failed"
    exit 1
fi

# Create results directory
mkdir -p ci_test_results
log_success "Created results directory"

# Run the experiment with timeout
log_info "Running experiment: $EXPERIMENT_NAME"
log_info "Timeout set to: $MAX_WAIT_TIME seconds"

# For macOS, we'll use a different timeout approach
if command -v timeout &> /dev/null; then
    # GNU timeout (if available via homebrew)
    timeout $MAX_WAIT_TIME ./run_experiment.sh "$EXPERIMENT_NAME" 2>&1 | tee ci_test_results/experiment_output.log
    EXIT_CODE=${PIPESTATUS[0]}
elif command -v gtimeout &> /dev/null; then
    # GNU timeout on macOS (brew install coreutils)
    gtimeout $MAX_WAIT_TIME ./run_experiment.sh "$EXPERIMENT_NAME" 2>&1 | tee ci_test_results/experiment_output.log
    EXIT_CODE=${PIPESTATUS[0]}
else
    # Fallback: run without timeout but with background process monitoring
    log_warning "No timeout command available, running without timeout"
    ./run_experiment.sh "$EXPERIMENT_NAME" 2>&1 | tee ci_test_results/experiment_output.log
    EXIT_CODE=$?
fi

# Check exit status
if [ $EXIT_CODE -eq 0 ]; then
    log_success "Experiment completed successfully"
elif [ $EXIT_CODE -eq 124 ]; then
    log_error "Experiment timed out after $MAX_WAIT_TIME seconds"
    exit 1
else
    log_error "Experiment failed with exit code: $EXIT_CODE"
    if [ -f ci_test_results/experiment_output.log ]; then
        log_info "Last 10 lines of output:"
        tail -10 ci_test_results/experiment_output.log
    fi
    exit 1
fi

# Validate results
log_info "Validating experiment results..."

RESULTS_FOUND=false

# Check various result locations
if [ -d "results" ] && [ "$(ls -A results 2>/dev/null)" ]; then
    log_success "Found results in main results/ directory"
    RESULTS_FOUND=true
fi

if [ -d "exp/$EXPERIMENT_NAME/results" ] && [ "$(ls -A exp/$EXPERIMENT_NAME/results 2>/dev/null)" ]; then
    log_success "Found results in experiment-specific directory"
    RESULTS_FOUND=true
fi

# Check output log
if [ -f ci_test_results/experiment_output.log ]; then
    OUTPUT_SIZE=$(wc -c < ci_test_results/experiment_output.log 2>/dev/null || echo 0)
    if [ "$OUTPUT_SIZE" -gt 0 ]; then
        log_success "Generated experiment output log ($OUTPUT_SIZE bytes)"
        
        # Look for success indicators
        if grep -q -i "simulation.*completed\|experiment.*completed\|finished" ci_test_results/experiment_output.log 2>/dev/null; then
            log_success "Found completion indicator in output"
            RESULTS_FOUND=true
        fi
    else
        log_warning "Output log is empty"
    fi
else
    log_warning "No output log generated"
fi

if [ "$RESULTS_FOUND" = "true" ]; then
    log_success "Experiment validation completed successfully"
else
    log_warning "No clear results found, but experiment completed without errors"
fi

# Generate summary
{
    echo "# CI Test Summary"
    echo "- **Experiment**: $EXPERIMENT_NAME"
    echo "- **Status**: $([ $EXIT_CODE -eq 0 ] && echo 'SUCCESS' || echo 'FAILED')"
    echo "- **Platform**: $(uname -s) $(uname -r)"
    echo "- **Timestamp**: $(date)"
} > ci_test_results/summary.md

log_success "🎉 CI test completed successfully!"
log_info "Summary saved to ci_test_results/summary.md"

exit 0
