#!/bin/bash

# Deploy CI/CD Setup Script
# This script commits and pushes the GitHub Actions CI/CD configuration

set -e

echo "🚀 Deploying omni_simulator CI/CD Setup"
echo "======================================="

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
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

# Check if we're in a git repository
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    log_error "Not in a git repository. Please initialize git first."
    exit 1
fi

# Check if we have any changes to commit
if git diff --quiet && git diff --cached --quiet; then
    log_warning "No changes detected. CI/CD files may already be committed."
    
    # Check if CI files exist
    if [ -d ".github" ]; then
        log_info "GitHub Actions files already exist"
        echo "Files found:"
        find .github -name "*.yml" -o -name "*.sh" | head -10
    else
        log_error "No .github directory found. Please run the CI setup first."
        exit 1
    fi
else
    log_info "Changes detected, preparing to commit..."
fi

# Show what will be committed
echo ""
log_info "Files to be committed:"
echo "====================="

# Check .github directory
if [ -d ".github" ]; then
    echo "📁 .github/"
    find .github -type f | sort | sed 's/^/   ├── /'
else
    log_error ".github directory not found!"
    exit 1
fi

# Check if README.md was modified
if git diff --name-only | grep -q "README.md" || git diff --cached --name-only | grep -q "README.md"; then
    echo "📄 README.md (CI badges added)"
fi

# Check for other documentation
for doc in "CI_SETUP_COMPLETE.md" "test_ci_setup.sh" "deploy_ci.sh"; do
    if [ -f "$doc" ]; then
        echo "📋 $doc"
    fi
done

echo ""
echo "🔍 Verification:"
echo "================"

# Count workflow files
workflow_count=$(find .github/workflows -name "*.yml" 2>/dev/null | wc -l | tr -d ' ')
script_count=$(find .github/scripts -name "*.sh" 2>/dev/null | wc -l | tr -d ' ')

echo "   ✅ Workflows: $workflow_count"
echo "   ✅ Scripts: $script_count"

# Check key files
key_files=(
    ".github/workflows/ci.yml"
    ".github/workflows/experiment-test.yml"
    ".github/scripts/run_experiment_test.sh"
)

missing_files=0
for file in "${key_files[@]}"; do
    if [ -f "$file" ]; then
        echo "   ✅ $file"
    else
        echo "   ❌ $file (missing)"
        missing_files=$((missing_files + 1))
    fi
done

if [ $missing_files -gt 0 ]; then
    log_error "$missing_files key files are missing!"
    exit 1
fi

echo ""
echo "📋 Commit Details:"
echo "=================="

COMMIT_MESSAGE="Add comprehensive GitHub Actions CI/CD support

Features:
- ✅ Experiment validation workflow (experiment-test.yml)
- ✅ Multi-platform CI pipeline (ci.yml) 
- ✅ Quick validation workflow (quick-test.yml)
- ✅ CI helper scripts for local testing
- ✅ Updated README.md with CI status badges
- ✅ Comprehensive documentation

Key validation:
- Ensures ./run_experiment.sh duato_on_ecube_hypercube runs successfully
- Tests across multiple GCC versions (9, 11)
- Validates on Ubuntu 20.04 and 22.04
- Automated build verification and result validation
- Artifact preservation with 30-day retention

CI triggers on every push and pull request to main/master/develop branches."

echo "$COMMIT_MESSAGE"

echo ""
read -p "❓ Do you want to commit and push these changes? (y/N): " -n 1 -r
echo

if [[ $REPLY =~ ^[Yy]$ ]]; then
    log_info "Staging files..."
    
    # Stage .github directory and related files
    git add .github/
    git add README.md
    
    # Add documentation files if they exist
    for doc in "CI_SETUP_COMPLETE.md" "test_ci_setup.sh" "deploy_ci.sh"; do
        if [ -f "$doc" ]; then
            git add "$doc"
        fi
    done
    
    log_success "Files staged"
    
    log_info "Committing changes..."
    git commit -m "$COMMIT_MESSAGE"
    log_success "Changes committed"
    
    # Get remote information
    if git remote -v | grep -q "origin"; then
        remote_url=$(git remote get-url origin)
        log_info "Remote origin: $remote_url"
        
        echo ""
        read -p "❓ Push to origin? (y/N): " -n 1 -r
        echo
        
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            log_info "Pushing to origin..."
            
            # Get current branch
            current_branch=$(git rev-parse --abbrev-ref HEAD)
            
            if git push origin "$current_branch"; then
                log_success "Successfully pushed to origin/$current_branch"
                
                # Extract GitHub repository info
                if echo "$remote_url" | grep -q "github.com"; then
                    repo_path=$(echo "$remote_url" | sed 's/.*github\.com[:/]\([^/]*\/[^/.]*\).*/\1/')
                    
                    echo ""
                    echo "🎉 CI/CD Setup Deployed Successfully!"
                    echo "===================================="
                    echo ""
                    echo "📊 Monitor CI Status:"
                    echo "   Actions: https://github.com/$repo_path/actions"
                    echo "   Workflows: https://github.com/$repo_path/actions/workflows"
                    echo ""
                    echo "🔍 Key Workflows:"
                    echo "   • Experiment Test: Tests duato_on_ecube_hypercube"
                    echo "   • CI Pipeline: Multi-environment validation"
                    echo "   • Quick Test: Fast validation feedback"
                    echo ""
                    echo "📋 What happens next:"
                    echo "   1. GitHub will automatically run CI on this push"
                    echo "   2. Check the Actions tab for real-time status"
                    echo "   3. CI badges in README.md will update automatically"
                    echo "   4. Future pushes/PRs will trigger automatic validation"
                    echo ""
                    echo "✅ Your project now has enterprise-grade CI/CD!"
                fi
            else
                log_error "Failed to push changes"
                exit 1
            fi
        else
            log_info "Skipped pushing to remote"
            echo "You can push later with: git push origin $(git rev-parse --abbrev-ref HEAD)"
        fi
    else
        log_warning "No remote 'origin' configured"
        echo "Add a remote with: git remote add origin <repository-url>"
    fi
else
    log_info "Deployment cancelled"
    echo "You can commit manually later with:"
    echo "  git add .github/ README.md"
    echo "  git commit -m 'Add GitHub Actions CI/CD support'"
    echo "  git push origin main"
fi

echo ""
log_success "Deploy script completed!"
