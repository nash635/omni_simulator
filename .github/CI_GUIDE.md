# GitHub Actions CI/CD Setup Guide

This document explains the CI/CD setup for the omni_simulator project.

## Workflow Overview

The project includes two main CI/CD workflows:

### 1. Experiment Test (`experiment-test.yml`)
**Primary Purpose**: Validates that the core experiment runs successfully
- **Trigger**: Every push and pull request
- **Test**: `./run_experiment.sh duato_on_ecube_hypercube`
- **Timeout**: 5 minutes maximum
- **Artifacts**: Saves experiment outputs and logs

### 2. Full CI Pipeline (`ci.yml`)
**Primary Purpose**: Comprehensive testing across multiple environments
- **Multiple GCC versions**: 9, 11
- **Cross-platform testing**: Ubuntu 20.04, 22.04
- **Build verification**: Clean and incremental builds
- **Configuration validation**: JSON syntax and project structure

## Badge Status

Add these badges to your README.md to show CI status:

```markdown
[![CI/CD Pipeline](https://github.com/nash635/omni_simulator/actions/workflows/ci.yml/badge.svg)](https://github.com/nash635/omni_simulator/actions/workflows/ci.yml)
[![Experiment Test](https://github.com/nash635/omni_simulator/actions/workflows/experiment-test.yml/badge.svg)](https://github.com/nash635/omni_simulator/actions/workflows/experiment-test.yml)
```

## Local Testing

Before pushing changes, you can run the same tests locally:

```bash
# Quick validation
.github/scripts/validate_ci.sh

# Full experiment test  
.github/scripts/run_experiment_test.sh

# macOS-specific test
.github/scripts/macos_test.sh
```

## Troubleshooting

### Common Issues

1. **Build Failures**
   - Check GCC version compatibility
   - Verify Bazel installation
   - Ensure all dependencies are installed

2. **Experiment Timeouts**
   - Current timeout is 5 minutes
   - Check if experiment configuration is correct
   - Review experiment parameters for complexity

3. **Artifact Upload Issues**
   - Ensure result directories exist
   - Check file permissions
   - Verify artifact paths in workflow

### Viewing Results

- **Live Status**: [Actions Tab](https://github.com/nash635/omni_simulator/actions)
- **Artifacts**: Download from completed workflow runs
- **Logs**: Available in workflow run details

## Maintenance

### Updating Workflows

1. **Adding New Experiments**: Update experiment test scripts
2. **Changing Timeouts**: Modify timeout values in workflows
3. **Adding Platforms**: Add new OS versions to matrix builds

### Security

- All workflows run in isolated environments
- No secrets are required for basic functionality
- Artifacts are automatically cleaned up after retention period
