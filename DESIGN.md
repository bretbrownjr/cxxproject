# Design

These principles guide the [proof-of-concept roadmap](ROADMAP.md). Decisions are
provisional until implementation tests them.

## Project interface

`cxx.json` would hold project metadata and declarations needed by shared tooling.
`cxxp info` would expose resolved project information as deterministic JSON,
including discovered files, configured tools, and the build backend. The manifest
and resolved output are separate interfaces.

## Boundaries

* Declare project intent rather than duplicate a build system's graph or flags.
* Reuse native tool configuration, such as `.clang-format` and `.clang-tidy`.
* Keep the project model independent of the build backend.
* Make diagnostics usable by humans and tools.
* Add shared concepts only when a concrete consumer needs them.

The interface should not prescribe a compiler or IDE.

## Decisions still open

Implementation must establish the manifest schema, file-discovery rules, how to
obtain compilation information for analysis, and command failure behavior.
See the [roadmap](ROADMAP.md) for deferred work.
