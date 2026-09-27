# Roadmap

## First proof of concept

The first proof of concept will work toward these capabilities in order:

1. **Foundation:** define and validate a minimal `cxx.json`, discover the project
   root, and provide a runnable `cxxp` with help and version output.
   See the [action plan](FOUNDATION.md).
2. **Introspection (complete):** implement `cxxp info` with deterministic JSON describing
   project metadata, discovered files, tool configurations, and the build backend.
   See the [command reference](CLI.md) for the implemented behavior.
3. **Formatting:** implement `cxxp format` and `--check` using clang-format and
   existing configuration, without requiring a build.
4. **Analysis:** implement `cxxp check` using clang-tidy and a compilation database,
   with useful failure behavior and optional SARIF output.
5. **Building:** implement `cxxp build` through CMake. Preserve direct CMake builds.

Use the tool on this repository as soon as it is practical.

## Later work

After the basic workflow works, evaluate configuration layering, C/C++ language
requirements, and an extensible build-backend interface. Set schema compatibility
rules before treating the `cxx.json` format or machine-readable output as stable.
