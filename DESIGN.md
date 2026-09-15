# cxxproject Design

This document captures the design goals, architectural decisions, and open design questions for `cxxproject`.

The project is currently an experimental proof of concept. Some decisions documented here are provisional and may change as the POC provides evidence about what works.

Open questions are also tracked in [ROADMAP.md](ROADMAP.md) when they represent planned design or implementation work.

## 1. Goals

`cxxproject` aims to provide a standardized, declarative project description and tooling interface for C and C++ projects.

The primary goals are:

1. Establish a project-level description independent of a particular build system.
2. Provide simple, consistent interfaces for common developer operations.
3. Make project information easily consumable by tools, IDEs, CI systems, and agents.
4. Reuse existing C and C++ tools rather than unnecessarily replacing them.
5. Establish abstractions that can eventually support multiple implementations.
6. Prefer project-level declarations of intent over build-system-specific implementation details.

The initial POC focuses on:

* project metadata
* project introspection
* source-file discovery
* formatting
* static analysis
* SARIF diagnostics
* building through CMake

## 2. Non-goals

`cxxproject` is not initially intended to:

* replace existing build systems
* define a new general-purpose build-system language
* reproduce the complete semantic model of CMake, Meson, Bazel, or other build systems
* prescribe a compiler or IDE
* replace established tool-specific configuration formats
* solve C/C++ dependency management

