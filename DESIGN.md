# Design

These principles guide the [proof-of-concept roadmap](ROADMAP.md). Decisions are
provisional until implementation tests them.

## Project interface

`cxx.json` holds project metadata; see the [file format reference](CXX_JSON.md) for
its format.

`cxxp info` exposes resolved project information as deterministic JSON, including
discovered files, tool configuration files, and the build backend. The `cxx.json`
file and resolved output are separate interfaces; see the [command reference](CLI.md).
The file inventory reports matching paths in the project tree, not the build
system's inputs. Tool entries report configuration file locations, not executable
availability or inherited settings.

## Boundaries

* Declare project intent rather than duplicate a build system's graph or flags.
* Reuse native tool configuration, such as `.clang-format` and `.clang-tidy`.
* Keep the project model independent of the build backend.
* Make diagnostics usable by humans and tools.
* Add shared concepts only when a concrete consumer needs them.

The interface should not prescribe a compiler or IDE.

## Decisions still open

File discovery and `info` failure behavior are defined by the initial command
contract. Analysis still needs a source for compilation information and useful
failure behavior. See the [roadmap](ROADMAP.md) for deferred work.
