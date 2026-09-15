# cxxproject

**A project structure and tooling standard for C and C++.**

> **Status: Experimental / POC**

`cxxproject` explores a standardized, declarative description of C and C++ projects that can be consumed by developer tools, build systems, IDEs, CI systems, and AI agents.

The initial project manifest is `cxx.json`. The initial command-line tool is `cxxp`.

The goal is not to replace existing C and C++ build systems. Instead, `cxxproject` aims to establish a common project-level interface that existing tools can understand and build upon.

## Why?

C and C++ projects have powerful build systems and developer tools, but relatively little standardization at the project level.

Basic operations frequently depend on the implementation details of a particular build system:

* enabling and validating compiler warnings
* running static analysis
* formatting source files
* discovering project files
* obtaining information about the project
* integrating diagnostics with editors and CI systems
* determining how a project is expected to be built

As a result, projects often encode this information in build-system-specific DSLs, shell scripts, CI configuration, editor configuration, and ad-hoc tooling.

This makes otherwise simple operations surprisingly difficult to standardize or reuse.

`cxxproject` explores a different approach:

> **Declare what the project is and what properties it requires, then let existing tools implement those requirements.**

For example, rather than treating a compiler warning as merely a compiler flag to pass to GCC, a project can eventually declare that it is expected to be free of diagnostics corresponding to that warning when supported by the compiler.

This distinction allows the project description to express intent without prescribing a particular compiler invocation.

## Inspiration

`cxxproject` takes inspiration from project metadata and tooling ecosystems such as Python's `pyproject.toml`, Rust's Cargo, and Go tooling.

In particular, Python's separation of project metadata and build frontends/backends is an important precedent. PEP 517 and PEP 518 established a model in which a standard project description can interact with independently implemented build backends.

The initial `cxxproject` implementation will use CMake as its build backend. Other build systems may be supported in the future.

## `cxx.json`

A `cxxproject` project contains a `cxx.json` manifest.

A minimal example:

```json
{
  "$schema": "https://cxxproject.dev/schema/0/cxx.json",

  "project": {
    "name": "example",
    "version": "0.1.0"
  },

  "build-system": {
    "backend": "cmake"
```

