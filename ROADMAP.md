# Roadmap

## First proof of concept

The first proof of concept will work toward these capabilities in order:

1. **Foundation: (complete)** define and validate a minimal `cxx.json`, discover the project
   root, and provide a runnable `cxxp` with help and version output.
2. **Introspection (complete):** implement `cxxp info` with deterministic JSON describing
   project metadata, discovered files, tool configurations, and the build backend.
   See the [command reference](CLI.md) for the implemented behavior.
3. **Formatting:** implement in-place `cxxp format` and `--check` using
   clang-format and existing configuration, without a build. Record and enforce
   the selected formatter version in `cxx.lock.json`, with automatic creation
   and explicit updates. Start with Linux.
   See the [command and lock contract](CLI.md#cxxp-format).
4. **Analysis:** implement `cxxp check` using clang-tidy and a compilation database,
   with useful failure behavior and optional SARIF output.
5. **Building:** implement `cxxp build` through CMake. Preserve direct CMake builds.

Use the tool on this repository as soon as it is practical.

## Later work

After the basic workflow works, evaluate configuration layering, C/C++ language
requirements, and an extensible build-backend interface. Set schema compatibility
rules before treating the `cxx.json` format or machine-readable output as stable.
Consider provisioning specific development tool versions through providers such
as PyPI or Conan once version locking works with installed tools.
