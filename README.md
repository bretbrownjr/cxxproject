# cxxproject

cxxproject is an experimental project interface for C and C++: a `cxx.json`
manifest for project metadata and a `cxxp` command intended to expose common
development operations.

## Motivation

C and C++ projects often scatter metadata and development workflows across build
files, scripts, CI, and editor settings. The intended result is that an editor or
CI system could ask `cxxp` for project information or request formatting through
the same interface across projects.

The manifest would describe project intent, while existing tools handle builds,
formatting, and analysis.

## Scope

The goal is a shared project interface, not a new build language or dependency
manager. Python's `pyproject.toml` and its frontend/backend separation are useful
precedents.

## Current status

Early prototype. `cxxp` currently provides help and version output, and a separate
library validates parsed manifests. Project discovery and command-line validation
are planned; see the [roadmap](ROADMAP.md).

## Getting involved

To build and explore the prototype, see [Development](DEVELOPMENT.md). Contributions
and concrete use cases are welcome; see [Contributing](CONTRIBUTING.md).

## Further reading

* [CXX_JSON.md](CXX_JSON.md) for the `cxx.json` reference
* [DESIGN.md](DESIGN.md) for a conceptual design explainer
* [ROADMAP.md](ROADMAP.md) for planned work
* [TODO.md](TODO.md) for implementation follow-ups and upstream questions

## License

Licensed under the [Apache License 2.0 with LLVM exceptions](LICENSE)
(`Apache-2.0 WITH LLVM-exception`).
