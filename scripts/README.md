# 🔧 Scripts

Utility scripts for building, testing, and maintaining the Air-Trap project.

## Available Scripts

| Script | Description |
|--------|-------------|
| `build.sh` | Main build script |
| `format.sh` | Code formatting with clang-format |
| `lint.sh` | Static analysis with clang-tidy |
| `install-hooks.sh` | Set up git hooks |
| `smart-commit.sh` | Guided commit helper |

## build.sh

### Usage

```bash
./scripts/build.sh [mode]
```

### Modes

| Mode | Description |
|------|-------------|
| (none) | Release build |
| `debug` | Debug build with symbols |
| `test` | Run unit tests |
| `coverage` | Generate coverage report |
| `clean` | Remove build directory |

### Examples

```bash
# Release build
./scripts/build.sh

# Debug build
./scripts/build.sh debug

# Run tests
./scripts/build.sh test

# Generate coverage
./scripts/build.sh coverage

# Clean build
./scripts/build.sh clean
```

### Output

- Release: `r-type_client`, `r-type_server`
- Debug: `r-type_client_debug`, `r-type_server_debug`

## format.sh

Formats all C++ source files using clang-format:

```bash
./scripts/format.sh
```

Configuration: `.clang-format`

## lint.sh

Runs static analysis:

```bash
./scripts/lint.sh
```

Configuration: `.clang-tidy`

## install-hooks.sh

Installs git pre-commit hooks for automatic formatting:

```bash
./scripts/install-hooks.sh
```

## smart-commit.sh

Interactive commit helper following project conventions:

```bash
./scripts/smart-commit.sh
```

See [SMART_COMMIT_GUIDE.md](../SMART_COMMIT_GUIDE.md) for details.

## Requirements

- Bash 4.0+
- CMake 3.20+
- Conan 2.x
- GCC 13+ or Clang 16+
- Python 3 (for Conan)
