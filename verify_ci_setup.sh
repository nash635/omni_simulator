#!/bin/bash

# CI Setup Verification Script
# This script verifies that all CI configuration is properly set up

echo "🔍 Verifying CI Setup for omni_simulator"
echo "========================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

error_count=0

# Function to print status
print_status() {
    local status=$1
    local message=$2
    if [ "$status" = "OK" ]; then
        echo -e "${GREEN}✅ $message${NC}"
    elif [ "$status" = "WARN" ]; then
        echo -e "${YELLOW}⚠️  $message${NC}"
    else
        echo -e "${RED}❌ $message${NC}"
        ((error_count++))
    fi
}

echo -e "${BLUE}1. Checking GitHub Actions workflows...${NC}"

# Check if .github/workflows directory exists
if [ -d ".github/workflows" ]; then
    print_status "OK" ".github/workflows directory exists"
    
    # Check each workflow file
    workflows=("ci.yml" "experiment-test.yml" "quick-test.yml" "simple-test.yml")
    for workflow in "${workflows[@]}"; do
        if [ -f ".github/workflows/$workflow" ]; then
            print_status "OK" "Workflow $workflow exists"
            
            # Check if it uses manual Bazel installation (not bazelisk)
            if grep -q "bazelbuild/setup-bazelisk" ".github/workflows/$workflow"; then
                print_status "ERROR" "Workflow $workflow still uses deprecated bazelisk setup"
            else
                print_status "OK" "Workflow $workflow uses stable Bazel installation"
            fi
            
            # Check if it uses updated action versions
            if grep -q "actions/upload-artifact@v4" ".github/workflows/$workflow"; then
                print_status "OK" "Workflow $workflow uses upload-artifact@v4"
            elif grep -q "actions/upload-artifact@v3" ".github/workflows/$workflow"; then
                print_status "WARN" "Workflow $workflow uses deprecated upload-artifact@v3"
            fi
            
        else
            print_status "ERROR" "Workflow $workflow missing"
        fi
    done
else
    print_status "ERROR" ".github/workflows directory missing"
fi

echo -e "\n${BLUE}2. Checking CI helper scripts...${NC}"

# Check CI scripts directory
if [ -d ".github/scripts" ]; then
    print_status "OK" ".github/scripts directory exists"
    
    scripts=("validate_ci.sh" "run_experiment_test.sh" "quick_test.sh" "verify_setup.sh")
    for script in "${scripts[@]}"; do
        if [ -f ".github/scripts/$script" ]; then
            print_status "OK" "CI script $script exists"
            if [ -x ".github/scripts/$script" ]; then
                print_status "OK" "CI script $script is executable"
            else
                print_status "WARN" "CI script $script needs execute permission"
            fi
        else
            print_status "ERROR" "CI script $script missing"
        fi
    done
else
    print_status "ERROR" ".github/scripts directory missing"
fi

echo -e "\n${BLUE}3. Checking project structure...${NC}"

# Check essential files
files=("run_experiment.sh" "BUILD" "WORKSPACE" "config.json")
for file in "${files[@]}"; do
    if [ -f "$file" ]; then
        print_status "OK" "Essential file $file exists"
    else
        print_status "ERROR" "Essential file $file missing"
    fi
done

# Check experiment configuration
if [ -f "exp/duato_on_ecube_hypercube/duato_on_ecube_hypercube.json" ]; then
    print_status "OK" "Experiment configuration exists"
    
    # Validate JSON
    if python3 -m json.tool exp/duato_on_ecube_hypercube/duato_on_ecube_hypercube.json > /dev/null 2>&1; then
        print_status "OK" "Experiment configuration is valid JSON"
    else
        print_status "ERROR" "Experiment configuration has invalid JSON"
    fi
else
    print_status "ERROR" "Experiment configuration missing"
fi

echo -e "\n${BLUE}4. Checking documentation...${NC}"

# Check documentation files
docs=("README.md" ".github/CI_GUIDE.md")
for doc in "${docs[@]}"; do
    if [ -f "$doc" ]; then
        print_status "OK" "Documentation $doc exists"
    else
        print_status "WARN" "Documentation $doc missing"
    fi
done

# Check if README has CI badges
if [ -f "README.md" ] && grep -q "github.com.*workflows.*badge" "README.md"; then
    print_status "OK" "README.md contains CI status badges"
else
    print_status "WARN" "README.md missing CI status badges"
fi

echo -e "\n${BLUE}5. Testing local build capability...${NC}"

# Check if we can test build locally
if command -v bazel >/dev/null 2>&1; then
    print_status "OK" "Bazel is available locally"
    
    # Test build syntax
    if bazel query //... >/dev/null 2>&1; then
        print_status "OK" "Bazel workspace is valid"
    else
        print_status "WARN" "Bazel workspace may have issues"
    fi
else
    print_status "WARN" "Bazel not available locally (CI will install it)"
fi

# Check if run_experiment.sh is executable
if [ -x "run_experiment.sh" ]; then
    print_status "OK" "run_experiment.sh is executable"
else
    print_status "WARN" "run_experiment.sh needs execute permission"
fi

echo -e "\n${BLUE}6. Summary${NC}"
echo "========="

if [ $error_count -eq 0 ]; then
    echo -e "${GREEN}🎉 CI setup verification completed successfully!${NC}"
    echo -e "${GREEN}✅ All critical components are in place${NC}"
    echo -e "${BLUE}💡 Ready to deploy to GitHub and run CI/CD pipeline${NC}"
else
    echo -e "${RED}❌ Found $error_count critical issues that need to be fixed${NC}"
    echo -e "${YELLOW}⚠️  Please address the errors above before deploying${NC}"
fi

echo -e "\n${BLUE}Next Steps:${NC}"
echo "1. Fix any errors shown above"
echo "2. Commit and push to GitHub repository"
echo "3. Verify CI/CD pipeline runs successfully"
echo "4. Monitor CI status badges in README.md"

exit $error_count
