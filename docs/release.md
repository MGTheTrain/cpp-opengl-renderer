# Release Process

## Versioning

Packages follow semantic versioning with environment-specific suffixes:

| Environment | Trigger | Version format | Example |
|---|---|---|---|
| Dev | PR to `main` | `x.y.z-beta-<run>` | `0.1.0-beta-42` |
| QA | Push to `main` | `x.y.z-alpha-<run>` | `0.1.0-alpha-42` |
| Release | Git tag `v*.*.*` | `x.y.z` | `0.1.0` |

## Creating a Release

**1. Update the `version` file:**
```
0.1.0
```

**2. Commit and push to `main`:**
```bash
git commit -m "chore: prepare release v0.1.0"
git push origin main
```

**3. Create and push an annotated tag:**
```bash
git tag -a v0.1.0 -m "Release v0.1.0

- Summary of changes"
git push origin v0.1.0
```

Pushing the tag triggers the release pipeline which builds all platforms, runs tests, and attaches the following packages to the GitHub Release:

| Platform | Format |
|---|---|
| Linux | `.deb`, `.tar.gz` |
| macOS | `.tar.gz` |
| Windows | `.zip` |
| Web | `.js`, `.wasm`, `.data` |

## Package Installation

**Linux (DEB):**
```bash
sudo dpkg -i cpp-opengl-renderer-0.1.0.deb
```

**Linux / macOS (TGZ):**
```bash
tar -xzf cpp-opengl-renderer-0.1.0.tar.gz -C /opt/mgtt/
```

**Windows (ZIP):**
Extract and run the installer or copy binaries manually.
