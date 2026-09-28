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

## Code organization

Project loading, validation, discovery, and information serialization live in
`src/cxxp` and are shared through the `cxxp_project_file` library. The executable
handles command parsing and presentation. Project loading remains independent
of external tool execution so inspection does not require development tools.

Tool integrations will use adapters for identification and tool-specific
operations. Shared orchestration will own project discovery and version policy;
platform-specific process handling will sit behind an internal interface.

## Shared tool policy

Project declarations, adapter compatibility, exact selected releases, and local
executable paths are separate concerns. Development tools and dependencies used
to produce the project's binary have distinct roles in shared lock metadata.

Operations will share lock validation and atomic writes. Automatic initialization
may add missing selections; replacing an existing selection requires an explicit
update. Metadata maintenance does not itself run source-changing operations.
Help and project inspection remain independent of locks and tool availability.

Release locking cannot guarantee identical artifacts or output: vendor patches,
adapter changes, and native configuration outside the project can affect results.
See the [lockfile reference](CXX_LOCK_JSON.md) for the planned representation and
[command reference](CLI.md#cxxp-format) for behavior.

## Decisions still open

File discovery and `info` failure behavior are defined by the initial command
contract. Analysis still needs a source for compilation information and useful
failure behavior. See the [roadmap](ROADMAP.md) for deferred work.
