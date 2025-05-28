#!/bin/bash

# CI/CD Setup Verification Script
# This script verifies that the GitHub Actions CI/CD setup is correct

echo "🔧 omni_simulator CI/CD Setup Verification"
echo "=========================================="

# Check if we're in a git repository
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    echo "❌ Not in a git repository"
    exit 1
fi

echo "✅ Git repository detected"

# Check required GitHub Actions files
required_workflows=(
    ".github/workflows/ci.yml"
    ".github/workflows/experiment-test.yml"
    ".github/workflows/quick-test.yml"
)

echo ""
echo "📋 Checking GitHub Actions workflows..."
for workflow in "${required_workflows[@]}"; do
    if [ -f "$workflow" ]; then
        echo "✅ $workflow"
    else
        echo "❌ $workflow (missing)"
    fi
done

# Check CI scripts
required_scripts=(
    ".github/scripts/validate_ci.sh"
    ".github/scripts/run_experiment_test.sh"
    ".github/scripts/quick_test.sh"
    ".github/scripts/macos_test.sh"
)

echo ""
echo "📋 Checking CI scripts..."
for script in "${required_scripts[@]}"; do
    if [ -f "$script" ] && [ -x "$script" ]; then
        echo "✅ $script (executable)"
    elif [ -f "$script" ]; then
        echo "⚠️  $script (exists but not executable)"
        chmod +x "$script"
        echo "   Fixed: made executable"
    else
        echo "❌ $script (missing)"
    fi
done

# Check project structure
echo ""
echo "📋 Checking project structure..."
required_files=(
    "run_experiment.sh"
    "BUILD"
    "WORKSPACE"
    "config.json"
    "exp/duato_on_ecube_hypercube/duato_on_ecube_hypercube.json"
)

for file in "${required_files[@]}"; do
    if [ -f "$file" ]; then
        echo "✅ $file"
    else
        echo "❌ $file (missing)"
    fi
done

# Check README.md for CI badges
echo ""
echo "📋 Checking README.md for CI badges..."
if grep -q "actions/workflows" README.md; then
    echo "✅ CI badges found in README.md"
else
    echo "⚠️  No CI badges found in README.md"
fi

# Validate experiment configuration
echo ""
echo "📋 Validating experiment configuration..."
config_file="exp/duato_on_ecube_hypercube/duato_on_ecube_hypercube.json"
if [ -f "$config_file" ]; then
    if command -v python3 &> /dev/null; then
        if python3 -m json.tool "$config_file" > /dev/null 2>&1; then
            echo "✅ Experiment configuration is valid JSON"
        else
            echo "❌ Invalid JSON in experiment configuration"
        fi
    else
        echo "⚠️  Python3 not found, skipping JSON validation"
    fi
else
    echo "❌ Experiment configuration file not found"
fi

# Git remotes check
echo ""
echo "📋 Checking git remotes..."
if git remote -v | grep -q "github.com"; then
    echo "✅ GitHub remote detected"
    GITHUB_REPO=$(git remote get-url origin | sed 's/.*github.com[:/]\([^/]*\/[^/]*\)\.git.*/\1/')
    echo "   Repository: $GITHUB_REPO"
else
    echo "⚠️  No GitHub remote found"
fi

echo ""
echo "🚀 Next Steps:"
echo "1. Commit and push your changes to GitHub"
echo "2. Check the Actions tab: https://github.com/$GITHUB_REPO/actions"
echo "3. Verify that workflows run successfully"
echo "4. Update badge URLs in README.md if needed"

echo ""
echo "📖 For more information, see .github/CI_GUIDE.md"

echo ""
echo "✅ CI/CD setup verification completed!"
