# GitHub Actions CI Setup Guide

This document explains the simplified CI setup for the omni_simulator project.

## Workflow Overview

The project uses a single, streamlined CI workflow:

### CI - Experiment Test (`ci.yml`)
**Primary Purpose**: Quick experiment testing and validation
- **Trigger**: Automatic on push/PR to main branches + manual trigger
- **Runtime**: Ubuntu latest with standard GCC
- **Core Test**: `./run_experiment.sh duato_on_ecube_hypercube`
- **Timeout**: 2 minutes maximum
- **Artifacts**: Saves experiment results

## Badge Status

Add this badge to your README.md to show CI status:

```markdown
[![CI - Experiment Test](https://github.com/nash635/omni_simulator/actions/workflows/ci.yml/badge.svg)](https://github.com/nash635/omni_simulator/actions/workflows/ci.yml)
```

## Manual Trigger

You can manually trigger the CI pipeline from the GitHub Actions tab by:
1. Go to **Actions** tab in your repository
2. Select **CI - Experiment Test** workflow
3. Click **Run workflow** button
4. Choose the branch and click **Run workflow**

## Experiment Validation

The CI pipeline automatically validates:
- ✅ Build system (Bazel) setup and compilation
- ✅ Core experiment execution (`duato_on_ecube_hypercube`)
- ✅ Result generation

## Artifacts

Each CI run preserves experiment results as artifacts.

## Local Testing

Before pushing changes, you can run the same test locally:

```bash
# Run the core experiment test
./run_experiment.sh duato_on_ecube_hypercube

# Build the project with Bazel
bazel build //...
```

## Troubleshooting

### Common Issues

1. **Build Failures**
   - Verify Bazel installation
   - Ensure all dependencies are installed

2. **Experiment Timeouts**
   - Current timeout is 2 minutes
   - Check experiment configuration

3. **Artifact Upload Issues**
   - Ensure result directories exist
   - Check file permissions

### Viewing Results

- **Live Status**: [Actions Tab](https://github.com/nash635/omni_simulator/actions)
- **Artifacts**: Download from completed workflow runs
- **Logs**: Available in workflow run details

## Maintenance

### Updating Workflows

1. **Adding New Experiments**: Update experiment test scripts
2. **Changing Timeouts**: Modify timeout values in workflows

### Security

- All workflows run in isolated environments
- No secrets are required for basic functionality
