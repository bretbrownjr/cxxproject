# cxxproject

This is an experiment in a common project description and tooling interface for C
and C++.

## Motivation

C and C++ projects often scatter metadata and development workflows across build
files, scripts, CI, and editor settings. `cxxproject` proposes a `cxx.json` manifest
and a `cxxp` command so developers, IDEs, CI systems, and agents can discover project
information and run common operations consistently.

The manifest would describe project intent, while existing tools handle builds,
formatting, and analysis.

## Scope

The goal is a shared project interface, not a new build language or dependency
manager. Python's `pyproject.toml` and its frontend/backend separation are useful
precedents.

## Further reading

See:

* [CXX_JSON.md](CXX_JSON.md) for the `cxx.json` reference
* [DESIGN.md](DESIGN.md) for a conceptual design explainer
* [ROADMAP.md](ROADMAP.md) for planned work
* [CONTRIBUTING.md](CONTRIBUTING.md) for how to contribute
* Open implementation issues and upstream questions not on the roadmap are recorded in [TODO.md](TODO.md).
