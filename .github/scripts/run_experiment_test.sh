#!/bin/bash

# Experiment test runner for CI
# This script runs the experiment with proper error handling and timeout

set -e

# Configuration
EXPERIMENT_NAME="duato_on_ecube_hypercube"
TIMEOUT_DURATION=300  # 5 minutes
RESULTS_DIR="ci_test_results"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}[$(date '+%H:%M:%S')] ℹ️  $1${NC}"
}

log_success() {
    echo -e "${GREEN}[$(date '+%H:%M:%S')] ✅ $1${NC}"
}

log_warning() {
    echo -e "${YELLOW}[$(date '+%H:%M:%S')] ⚠️  $1${NC}"
}

log_error() {
    echo -e "${RED}[$(date '+%H:%M:%S')] ❌ $1${NC}"
}

# Create results directory
mkdir -p "$RESULTS_DIR"

log_info "Starting experiment test: $EXPERIMENT_NAME"
log_info "Timeout set to: $TIMEOUT_DURATION seconds"
log_info "Results directory: $RESULTS_DIR"

# Pre-flight checks
log_info "Running pre-flight checks..."

if [ ! -f "run_experiment.sh" ]; then
    log_error "run_experiment.sh not found"
    exit 1
fi

if [ ! -x "run_experiment.sh" ]; then
    log_warning "Making run_experiment.sh executable"
    chmod +x run_experiment.sh
fi

if [ ! -d "exp/$EXPERIMENT_NAME" ]; then
    log_error "Experiment directory not found: exp/$EXPERIMENT_NAME"
    exit 1
fi

if [ ! -f "exp/$EXPERIMENT_NAME/$EXPERIMENT_NAME.json" ]; then
    log_error "Experiment configuration not found: exp/$EXPERIMENT_NAME/$EXPERIMENT_NAME.json"
    exit 1
fi

log_success "Pre-flight checks completed"

# Run the experiment with timeout
log_info "Starting experiment execution..."

# Capture start time
START_TIME=$(date +%s)

# Run experiment with timeout and capture output
if timeout "$TIMEOUT_DURATION" ./run_experiment.sh "$EXPERIMENT_NAME" 2>&1 | tee "$RESULTS_DIR/experiment_output.log"; then
    EXIT_CODE=0
    log_success "Experiment completed successfully"
else
    EXIT_CODE=$?
    if [ $EXIT_CODE -eq 124 ]; then
        log_error "Experiment timed out after $TIMEOUT_DURATION seconds"
    else
        log_error "Experiment failed with exit code: $EXIT_CODE"
    fi
fi

# Capture end time and calculate duration
END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))

log_info "Experiment duration: ${DURATION} seconds"

# Analyze results
log_info "Analyzing experiment results..."

# Check if output log exists and has content
if [ -f "$RESULTS_DIR/experiment_output.log" ] && [ -s "$RESULTS_DIR/experiment_output.log" ]; then
    OUTPUT_LINES=$(wc -l < "$RESULTS_DIR/experiment_output.log")
    log_success "Generated output log with $OUTPUT_LINES lines"
    
    # Look for success indicators
    if grep -q -i "simulation.*completed\|experiment.*completed\|finished\|done" "$RESULTS_DIR/experiment_output.log"; then
        log_success "Found completion indicator in output"
    else
        log_warning "No clear completion indicator found"
    fi
    
    # Look for error indicators
    if grep -q -i "error\|failed\|exception" "$RESULTS_DIR/experiment_output.log"; then
        log_warning "Found potential error indicators in output"
    fi
    
    # Look for performance metrics
    if grep -q -i "latency\|throughput\|average\|performance" "$RESULTS_DIR/experiment_output.log"; then
        log_success "Found performance metrics in output"
    fi
    
else
    log_warning "No output log generated or log is empty"
fi

# Check for results files
log_info "Checking for generated results..."

result_locations=(
    "results/"
    "exp/$EXPERIMENT_NAME/results/"
    "$RESULTS_DIR/"
)

FOUND_RESULTS=false
for location in "${result_locations[@]}"; do
    if [ -d "$location" ] && [ "$(ls -A "$location" 2>/dev/null)" ]; then
        log_success "Found results in: $location"
        ls -la "$location"
        FOUND_RESULTS=true
    fi
done

if [ "$FOUND_RESULTS" = false ]; then
    log_warning "No result files found in expected locations"
fi

# Generate summary
log_info "Generating test summary..."

{
    echo "# Experiment Test Summary"
    echo ""
    echo "**Experiment**: $EXPERIMENT_NAME"
    echo "**Duration**: ${DURATION} seconds"
    echo "**Exit Code**: $EXIT_CODE"
    echo "**Timestamp**: $(date)"
    echo ""
    
    if [ -f "$RESULTS_DIR/experiment_output.log" ]; then
        echo "## Output Summary (Last 10 lines)"
        echo '```'
        tail -10 "$RESULTS_DIR/experiment_output.log"
        echo '```'
    fi
    
} > "$RESULTS_DIR/test_summary.md"

log_success "Test summary saved to: $RESULTS_DIR/test_summary.md"

# Final status
if [ $EXIT_CODE -eq 0 ]; then
    log_success "🎉 Experiment test completed successfully!"
    exit 0
else
    log_error "💥 Experiment test failed!"
    exit $EXIT_CODE
fi
