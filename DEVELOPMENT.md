# Development

## Required tools and dependencies

Building requires:

* CMake 4.4 or newer
* a C++23 compiler with `#embed` support
* a build tool supported by the chosen CMake generator

Packages required for select ecosystems:

| Ecosystem | Packages |
| --- | --- |
| Debian | [`nlohmann-json3-dev`][debian-nlohmann-json3-dev], [`libvalijson-dev`][debian-libvalijson-dev], [`catch2`][debian-catch2] |
| Conan | [`nlohmann_json`][conan-nlohmann_json], [`valijson`][conan-valijson], [`catch2`][conan-catch2], [`cmake`][conan-cmake], [`ninja`][conan-ninja] |
| vcpkg | [`nlohmann-json`][vcpkg-nlohmann-json], [`valijson`][vcpkg-valijson], [`catch2`][vcpkg-catch2] |

| Dependency | Required version |
| --- | --- |
| nlohmann/json | 3.11 or newer |
| Valijson | 1.0 or newer |
| Catch2 (tests only) | 2.13 or newer |

### With Debian packages

Install the packages listed above using your package manager. Check that the
available tool and dependency versions meet the requirements above. CMake can discover packages installed
in standard system locations.

### With Conan

The repository provides a [Conan recipe](conanfile.py). Standard Conan 2 workflows
are supported.

Follow Conan's [CMake integration guidance][conan-cmake-guide] for
how to manage profiles, dependency installation, generated toolchains, and CMake presets.

The recipe's project-specific defaults and experimental CPS integration are kept in
the recipe.

### With vcpkg

Install the listed ports and configure CMake with vcpkg's toolchain, composing it
with your compiler settings as needed. Select versions that meet the requirements
above. The repository does not currently provide a vcpkg
manifest or baseline.

## With CMake and CTest

Once dependencies are installed and discoverable, configure, build, and test:

```sh
cmake -S . -B build --toolchain /absolute/path/to/toolchain.cmake
cmake --build build --parallel
ctest --test-dir build --output-on-failure --no-tests=error
```

Use a fresh build directory when changing toolchains. Set `BUILD_TESTING=OFF`
when configuring a build without tests; Catch2 is then unnecessary.
The manifest tests use Catch2 scenarios discovered automatically by CTest.

The executable is `build/src/cxxp`; run it without arguments or with `--help` for
usage, or use `--version` for the CMake project version.

## With clang-format

Format C++ sources and headers using [.clang-format](.clang-format), which selects
LLVM style with east-const qualifiers (`T const`). From the repository root:

```sh
find src tests -type f \( -name '*.cxx' -o -name '*.hxx' \) -exec clang-format -i {} +
```

Formatting updates files in place. Review the resulting changes before committing.

## With just

The repository's `Justfile` provides short commands for the common development
workflows. Run `just` to list them.

### Code Formatting

```sh
just format
```

### Building the Project

```sh
just build
```

### Running Tests

```sh
just test
```

This command will update the build of the project before running tests.

### Regression Testing

To do a comprehensive check for issues:

```sh
just ci
```

`just ci` is suitable for local validation and Git hooks. It formats files in
place but does not stage the changes, so review the diff before committing.

Pass `toolchain=PATH` or set `CMAKE_TOOLCHAIN_FILE` when CMake needs a toolchain:

```sh
just toolchain=/absolute/path/to/toolchain.cmake ci
```

Use `build_dir=PATH` to select a different build directory. When Conan or another
tool has generated matching CMake presets, pass `preset=NAME`; this selects the
configure, build, and test presets instead of the direct build-directory workflow:

```sh
just preset=conan-debug ci
```

[debian-nlohmann-json3-dev]: https://packages.debian.org/nlohmann-json3-dev
[debian-libvalijson-dev]: https://packages.debian.org/libvalijson-dev
[debian-catch2]: https://packages.debian.org/catch2
[conan-nlohmann_json]: https://conan.io/center/recipes/nlohmann_json
[conan-valijson]: https://conan.io/center/recipes/valijson
[conan-catch2]: https://conan.io/center/recipes/catch2
[conan-cmake]: https://conan.io/center/recipes/cmake
[conan-ninja]: https://conan.io/center/recipes/ninja
[conan-cmake-guide]: https://docs.conan.io/2/integrations/cmake.html
[vcpkg-nlohmann-json]: https://vcpkg.io/en/package/nlohmann-json
[vcpkg-valijson]: https://vcpkg.io/en/package/valijson.html
[vcpkg-catch2]: https://vcpkg.io/en/package/catch2.html
