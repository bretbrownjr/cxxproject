# Contributing to cxxproject

Thank you for contributing to cxxproject.

cxxproject is an experimental project structure and tooling standard for C and C++. The project is intentionally small and opinionated. Contributions should generally improve the usefulness, portability, or clarity of the project without prematurely expanding its scope.

## Development Status

cxxproject is currently a proof of concept. APIs, the `cxx.json` schema, command behavior, and internal architecture may change.

Before making substantial changes, read:

* [`README.md`](README.md) — project overview and current capabilities
* [`DESIGN.md`](DESIGN.md) — design principles and architectural decisions
* [`ROADMAP.md`](ROADMAP.md) — planned milestones and open questions

For changes that affect the project model or manifest schema, `DESIGN.md` should be updated alongside the implementation.

## Building

The project is built with CMake.

A typical development build is:

```sh
cmake -S . -B build
cmake --build build
```

Tests can be run with:

```sh
ctest --test-dir build
```

The repository should remain directly buildable with CMake even as `cxxp` develops its own build frontend.

## Development Workflow

A typical workflow is:

1. Create a branch for the change.
2. Make the smallest change that addresses the problem.
3. Add or update tests.
4. Update documentation when behavior or design changes.
5. Build and run the test suite.
6. Run the project's own tooling where applicable.
7. Review the resulting diff before submitting the change.

The repository is intended to dogfood `cxxp` as its capabilities become available.

## Design Changes

Changes to the project model deserve particular care.

Before adding a new concept to `cxx.json`, consider:

* What problem does the concept solve?
* Who consumes the information?
* Could an existing tool or configuration format provide it instead?
* Does the concept describe project intent or merely a build-system implementation detail?
* Does it need to be standardized at all?
* How does it interact with other build systems?

Avoid adding fields merely because an equivalent concept exists in CMake or another build system.

When a proposed change represents a significant design decision, document the rationale in [`DESIGN.md`](DESIGN.md) and, where appropriate, add it to the open questions or milestones in [`ROADMAP.md`](ROADMAP.md).

## Manifest and Schema

The `cxx.json` manifest is a public interface.

Changes to its schema should:

* Update the schema definition.
* Include validation tests.
* Update relevant documentation and examples.
* Consider compatibility with existing manifests.
* Respect the manifest schema versioning strategy.

Do not silently introduce incompatible changes to an existing schema version.

The resolved output of `cxxp info` is also intended to become a stable machine-readable interface. Changes to that output should therefore be treated separately from changes to the input manifest.

## Tool Integrations

Tool integrations should generally expose the native tool rather than unnecessarily reproducing its configuration.

For example, clang-format configuration should initially use `.clang-format` rather than duplicating all clang-format options in `cxx.json`.

New tools should have their own namespace:

```json
{
  "tools": {
    "clang-format": {},
    "clang-tidy": {}
  }
}
```

Avoid generic configuration abstractions when the semantics belong to a specific tool.

## Diagnostics

Diagnostics are intended to support both human-readable output and machine-readable formats such as SARIF.

New diagnostic-producing functionality should avoid coupling the internal representation directly to terminal formatting. Where practical, diagnostics should pass through a common representation that can support multiple output formats.

## Build Backends

CMake is currently the only build backend.

The long-term design should allow additional backends without requiring changes to the core project model. Until that interface is established, avoid introducing CMake-specific concepts into the general `cxx.json` model.

## Tests

Tests should cover externally observable behavior where practical.

Prefer tests that verify:

* manifest parsing and validation
* project discovery
* resolved project information
* command behavior and exit status
* tool discovery and invocation
* diagnostic output
* schema compatibility
* build backend behavior

Changes should not rely solely on manual testing when the behavior can reasonably be automated.

## Code Style

Follow the existing style of the codebase.

When formatting support is available, use the project's configured formatter rather than introducing unrelated formatting changes.

Keep changes focused. Avoid drive-by refactoring unless it is necessary for the change.

## Pull Requests

Pull requests should explain:

* What changed.
* Why the change is needed.
* Any relevant design decisions.
* How the change was tested.
* Any remaining limitations or open questions.

For changes affecting the manifest, project model, or command-line interface, include representative examples where useful.

Large changes should generally be broken into smaller, independently understandable changes when possible.

## Design Philosophy

The following principles should guide contributions:

* **Declare intent, not implementation.**
* **Prefer interoperable project information over build-system-specific configuration.**
* **Use existing tools rather than unnecessarily replacing them.**
* **Keep the initial standard small and opinionated.**
* **Make machine-readable interfaces first-class.**
* **Do not standardize concepts without a demonstrated consumer or benefit.**

The goal is useful standardization, not comprehensive standardization.

