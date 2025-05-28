#!/bin/bash

# Simple CI test for duato_on_ecube_hypercube experiment
# This script performs a quick validation that the experiment can run

set -e

echo "🧪 Testing duato_on_ecube_hypercube experiment for CI..."

# Check if experiment script exists
if [ ! -f "run_experiment.sh" ]; then
    echo "❌ run_experiment.sh not found"
    exit 1
fi

# Make script executable
chmod +x run_experiment.sh

# Check if experiment config exists  
if [ ! -f "exp/duato_on_ecube_hypercube/duato_on_ecube_hypercube.json" ]; then
    echo "❌ Experiment configuration not found"
    exit 1
fi

# Verify JSON config is valid
if ! python3 -m json.tool exp/duato_on_ecube_hypercube/duato_on_ecube_hypercube.json > /dev/null; then
    echo "❌ Invalid JSON configuration"
    exit 1
fi

echo "✅ All pre-checks passed"

# Run the experiment with timeout (5 minutes max)
echo "🚀 Running experiment..."
timeout 300 ./run_experiment.sh duato_on_ecube_hypercube

# Check exit status
if [ $? -eq 0 ]; then
    echo "✅ Experiment completed successfully"
elif [ $? -eq 124 ]; then
    echo "⚠️ Experiment timed out (5 minutes)"
    exit 1
else
    echo "❌ Experiment failed"
    exit 1
fi

echo "🎉 CI test passed!"
