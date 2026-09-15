# Building and testing

The build requires CMake 4.4 or newer and a C++ compiler with C++23 support.
The language standard and ABI-sensitive flags belong in your toolchain file;
the project does not set them.

On Debian/Crostini, install the JSON dependencies with:

```sh
sudo apt install nlohmann-json3-dev libvalijson-dev
```

The tested environment uses GCC 12.2, CMake 4.4.3, nlohmann/json 3.11.2
(Debian `3.11.2-2`), and Valijson 1.0 (Debian `1.0+repack-2`). CMake finds
installed packages; configuration, compilation, and tests need no network access.
To reproduce the dependency versions, use those package versions from a matching
Debian repository or snapshot.

In the current development environment, tools are under `/opt/uv/.venv`:

```sh
export PATH=/opt/uv/.venv/bin:"$PATH"
```

## `just` Support

If you have `just` installed, `just toolchain=/absolute/path/to/toolchain.cmake test`
configures the project, builds it, and runs the tests. Run `just` to see the available
workflows.

If the environment's default temporary directory is read-only, add
`--tempdir /tmp` to the `just` command.

## CMake Support

With a toolchain file that selects your compiler and C++23 mode:

```sh
cmake -S . -B build --toolchain /absolute/path/to/toolchain.cmake
cmake --build build --parallel
ctest --test-dir build --output-on-failure --no-tests=error
```

Use a fresh build directory when changing toolchains. The executable is
`build/src/cxxp`; run it without arguments or with `--help` for usage, or use
`--version` for the CMake project version.
