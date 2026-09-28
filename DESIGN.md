# Design

These principles guide the [proof-of-concept roadmap](ROADMAP.md).

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
handles command parsing and presentation. Project loading is independent
of external tool execution so inspection does not require development tools.

Tool adapters handle identification and tool-specific operations. Project
discovery and version policy belong in shared orchestration. Platform-specific
process handling sits behind an internal interface. Tool execution is separate
from CLI parsing, project loading, and version policy.

## Shared tool policy

Project declarations, adapter compatibility, exact selected releases, and local
executable paths are separate concerns. Development tools and dependencies used
to produce the project's binary have distinct roles in shared lock metadata.

Lock validation and atomic writes belong in shared operations. Automatic initialization
may add missing selections; replacing an existing selection requires an explicit
update. Metadata maintenance does not itself run source-changing operations.
Help and project inspection are independent of locks and tool availability.

Release locking cannot guarantee identical artifacts or output: vendor patches,
adapter changes, and native configuration outside the project can affect results.
See the [lockfile reference](CXX_LOCK_JSON.md) for the representation and
[command reference](CLI.md#cxxp-format) for behavior.

## Open questions

The command contract defines file discovery and `info` failure behavior.
The source of compilation information and failure behavior for analysis are
open questions. See the [roadmap](ROADMAP.md) for planned capabilities.
