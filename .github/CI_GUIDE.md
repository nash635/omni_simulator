# GitHub Actions CI/CD Setup Guide

This document explains the simplified CI/CD setup for the omni_simulator project.

## Workflow Overview

The project uses a single, streamlined CI workflow:

### CI/CD Pipeline (`ci.yml`)
**Primary Purpose**: Comprehensive experiment testing and validation
- **Trigger**: Automatic on push/PR to main branches + manual trigger
- **Test Matrix**: GCC versions 9 and 11
- **Core Test**: `./run_experiment.sh duato_on_ecube_hypercube`
- **Timeout**: 5 minutes maximum
- **Artifacts**: Saves experiment outputs, logs, and results
- **Features**:
  - Automated Bazel installation and build
  - Experiment environment validation
  - Result analysis and summary generation
  - Detailed test reports with GitHub step summaries

## Badge Status

Add this badge to your README.md to show CI status:

```markdown
[![CI/CD Pipeline](https://github.com/nash635/omni_simulator/actions/workflows/ci.yml/badge.svg)](https://github.com/nash635/omni_simulator/actions/workflows/ci.yml)
```

## Manual Trigger

You can manually trigger the CI pipeline from the GitHub Actions tab by:
1. Go to **Actions** tab in your repository
2. Select **CI/CD Pipeline** workflow
3. Click **Run workflow** button
4. Choose the branch and click **Run workflow**

## Experiment Validation

The CI pipeline automatically validates:
- ✅ Build system (Bazel) setup and compilation
- ✅ Experiment configuration files (JSON syntax)
- ✅ Core experiment execution (`duato_on_ecube_hypercube`)
- ✅ Result generation and analysis
- ✅ System compatibility across GCC versions

## Artifacts

Each CI run preserves important artifacts for 30 days:
- Experiment output logs
- Performance analysis results
- Generated CSV files and charts
- Build and error logs

## Local Testing

Before pushing changes, you can run the same test locally:

```bash
# Run the core experiment test
./run_experiment.sh duato_on_ecube_hypercube

# Build the project with Bazel
bazel build //... --verbose_failures
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
