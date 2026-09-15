build_dir := "build"
toolchain := env_var_or_default("CMAKE_TOOLCHAIN_FILE", "")

# List the available workflows.
default:
    @just --list

# Configure CMake, optionally using CMAKE_TOOLCHAIN_FILE or toolchain=PATH.
configure:
    #!/usr/bin/env sh
    set -eu
    set --
    if [ -n {{ quote(toolchain) }} ]; then
        set -- {{ quote("-DCMAKE_TOOLCHAIN_FILE=" + toolchain) }}
    fi
    cmake -S . -B {{ quote(build_dir) }} "$@"

# Configure and build the project.
build: configure
    cmake --build {{ quote(build_dir) }} --parallel

# Build and run the CTest suite, showing output on failure.
test: build
    ctest --test-dir {{ quote(build_dir) }} --output-on-failure --no-tests=error
