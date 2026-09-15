# cxxproject

This is an experiment in a common project description and tooling interface for C
and C++. This repository currently contains the proposal. Implementation has not
started.

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

See [DESIGN.md](DESIGN.md) for boundaries, [ROADMAP.md](ROADMAP.md) for planned work,
and [CONTRIBUTING.md](CONTRIBUTING.md) for contribution guidance.
