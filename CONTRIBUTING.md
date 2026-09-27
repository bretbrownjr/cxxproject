# Contributing

Contributions are welcome, including concrete C and C++ workflow examples, bug
reports, documentation improvements, and code. This is an early prototype, so
feedback on the project interface is especially useful.

## Getting started

Start with the [project overview](README.md) and [design](DESIGN.md) to understand
the scope. The [roadmap](ROADMAP.md) describes planned work, and
[Development](DEVELOPMENT.md) explains how to build and test the prototype.

## Proposing changes

Keep changes small and focused on a concrete use case. For a new concept, explain
who needs it and why existing build or tool configuration is insufficient.

For bug reports, include steps to reproduce, expected and actual behavior, and
relevant tool and platform versions.

## Validation

Run the checks described in [Development](DEVELOPMENT.md#regression-testing) and
review any formatting changes. Include tests for new or changed behavior;
`cxx.json` changes should update the schema, examples, and validation tests together.

## Pull requests

Describe the problem, how the change addresses it, and how you validated it.
Update affected documentation, keeping planned features distinct from implemented
behavior and recording design decisions in [DESIGN.md](DESIGN.md).

Every pull request, including documentation changes, must increase the version in
the root CMake `project()` declaration relative to the base branch.

## License

Unless explicitly stated otherwise, contributions are submitted under the
project's [Apache License 2.0 with LLVM exceptions](LICENSE).
