#!/bin/bash

# CI validation script for omni_simulator
# This script ensures the experiment can run correctly in CI environment

set -e

echo "🔍 Starting omni_simulator CI validation..."

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
log_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

log_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

log_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

log_error() {
    echo -e "${RED}❌ $1${NC}"
}

# Check system requirements
log_info "Checking system requirements..."

# Check GCC version
if command -v gcc &> /dev/null; then
    GCC_VERSION=$(gcc --version | head -n1)
    log_success "GCC found: $GCC_VERSION"
else
    log_error "GCC not found"
    exit 1
fi

# Check Bazel
if command -v bazel &> /dev/null; then
    BAZEL_VERSION=$(bazel --version)
    log_success "Bazel found: $BAZEL_VERSION"
else
    log_error "Bazel not found"
    exit 1
fi

# Check Python
if command -v python3 &> /dev/null; then
    PYTHON_VERSION=$(python3 --version)
    log_success "Python found: $PYTHON_VERSION"
else
    log_error "Python3 not found"
    exit 1
fi

# Validate project structure
log_info "Validating project structure..."

required_files=(
    "BUILD"
    "WORKSPACE"
    "config.json"
    "run_experiment.sh"
    "exp/duato_on_ecube_hypercube/duato_on_ecube_hypercube.json"
)

for file in "${required_files[@]}"; do
    if [ -f "$file" ] || [ -d "$file" ]; then
        log_success "Found: $file"
    else
        log_error "Missing: $file"
        exit 1
    fi
done

# Validate JSON configurations
log_info "Validating JSON configurations..."

json_files=(
    "config.json"
    "exp/duato_on_ecube_hypercube/duato_on_ecube_hypercube.json"
)

for json_file in "${json_files[@]}"; do
    if python3 -m json.tool "$json_file" &> /dev/null; then
        log_success "Valid JSON: $json_file"
    else
        log_error "Invalid JSON: $json_file"
        exit 1
    fi
done

# Check script permissions
log_info "Checking script permissions..."

if [ -x "run_experiment.sh" ]; then
    log_success "run_experiment.sh is executable"
else
    log_warning "run_experiment.sh needs execute permission, fixing..."
    chmod +x run_experiment.sh
    log_success "Execute permission added"
fi

# Validate script syntax
log_info "Validating script syntax..."

if bash -n run_experiment.sh; then
    log_success "Script syntax is valid"
else
    log_error "Script syntax error found"
    exit 1
fi

# Test Bazel build
log_info "Testing Bazel build..."

if bazel build //... --verbose_failures; then
    log_success "Bazel build successful"
else
    log_error "Bazel build failed"
    exit 1
fi

# Prepare for experiment run
log_info "Preparing experiment environment..."

# Create results directory if it doesn't exist
mkdir -p results

# Backup original config if exists
if [ -f "config.json" ]; then
    cp config.json config.json.backup
    log_success "Configuration backed up"
fi

log_success "CI validation completed successfully!"
log_info "System is ready for experiment execution"

echo ""
echo "📊 System Summary:"
echo "  - GCC: $(gcc --version | head -n1 | cut -d' ' -f4)"
echo "  - Bazel: $(bazel --version | cut -d' ' -f2)"
echo "  - Python: $(python3 --version | cut -d' ' -f2)"
echo "  - OS: $(uname -s) $(uname -r)"
echo "  - Architecture: $(uname -m)"
