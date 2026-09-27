build_dir := "build"
toolchain := env_var_or_default("CMAKE_TOOLCHAIN_FILE", "")
preset := ""

# List the available workflows.
default:
    @just --list

# Format C++ sources and headers using .clang-format.
format:
    find src tests -type f \( -name '*.cxx' -o -name '*.hxx' \) -exec clang-format -i {} +

# Format, build, and run the test suite.
ci: format test

# Configure CMake, optionally using CMAKE_TOOLCHAIN_FILE or toolchain=PATH.
configure:
    #!/usr/bin/env sh
    set -eu
    if [ -n {{ quote(preset) }} ]; then
        cmake --preset {{ quote(preset) }}
        exit
    fi
    set --
    if [ -n {{ quote(toolchain) }} ]; then
        set -- {{ quote("-DCMAKE_TOOLCHAIN_FILE=" + toolchain) }}
    fi
    cmake -S . -B {{ quote(build_dir) }} "$@"

# Configure and build the project.
build: configure
    #!/usr/bin/env sh
    set -eu
    if [ -n {{ quote(preset) }} ]; then
        cmake --build --preset {{ quote(preset) }} --parallel
    else
        cmake --build {{ quote(build_dir) }} --parallel
    fi

# Build and run the CTest suite, showing output on failure.
test: build
    #!/usr/bin/env sh
    set -eu
    if [ -n {{ quote(preset) }} ]; then
        ctest --preset {{ quote(preset) }} --output-on-failure --no-tests=error
    else
        ctest --test-dir {{ quote(build_dir) }} --output-on-failure --no-tests=error
    fi
