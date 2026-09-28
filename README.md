# cxxproject

cxxproject is an experimental project interface for C and C++: a `cxx.json`
file for project metadata and a `cxxp` command intended to expose common
development operations.

## Motivation

C and C++ projects often scatter metadata and development workflows across build
files, scripts, CI, and editor settings. The intended result is that an editor or
CI system could ask `cxxp` for project information or request formatting through
the same interface across projects.

The `cxx.json` file would describe project intent, while existing tools handle builds,
formatting, and analysis.

## Scope

The goal is a shared project interface, not a new build language or dependency
manager. Python's `pyproject.toml` and its frontend/backend separation are useful
precedents.

## Current status

Early prototype. See `cxxp --help` for supported features.
Also see the [usage guide](USAGE.md), [command reference](CLI.md), and
[roadmap](ROADMAP.md).

## Getting involved

To build and explore the prototype, see [Development](DEVELOPMENT.md). Contributions
and concrete use cases are welcome; see [Contributing](CONTRIBUTING.md).

## Further reading

* [CXX_JSON.md](CXX_JSON.md) for the `cxx.json` reference
* [USAGE.md](USAGE.md) for common command workflows
* [CLI.md](CLI.md) for command syntax and behavior
* [DESIGN.md](DESIGN.md) for a conceptual design explainer
* [ROADMAP.md](ROADMAP.md) for planned work
* [TODO.md](TODO.md) for incidental tasks and future work

## License

Licensed under the [Apache License 2.0 with LLVM exceptions](LICENSE)
(`Apache-2.0 WITH LLVM-exception`).
