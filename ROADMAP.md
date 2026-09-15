# cxxproject Roadmap

This roadmap describes the planned progression of the `cxxproject` proof of concept.

The roadmap is intentionally capability-oriented. Individual implementation tasks may change as the design evolves.

Open design questions that require resolution are included as roadmap work rather than being treated as permanent notes.

## M0 — Project Foundation

Establish the basic `cxxproject` project model and a functioning `cxxp` executable.

### Deliverables

* [ ] Establish repository structure and development workflow.
* [ ] Define the initial `cxx.json` schema.
* [ ] Implement JSON parsing and schema validation.
* [ ] Implement project-root discovery.
* [ ] Implement basic project metadata:

  * [ ] project name
  * [ ] project version
  * [ ] schema version
* [ ] Implement `cxxp --version`.
* [ ] Implement `cxxp --help`.
* [ ] Add automated tests for manifest parsing and project discovery.
* [ ] Make the `cxxproject` repository itself a valid `cxxproject` project.

### Exit criteria

A fresh checkout can be built and run without manually invoking implementation-specific tooling beyond the documented bootstrap process.

Given a valid `cxx.json`, `cxxp` can:

1. locate the project,
2. parse the manifest,
3. validate it against the schema,
4. report project name and version, and
5. produce useful errors for invalid manifests.

---

## M1 — Project Introspection

Implement `cxxp info` as the first substantial user-facing capability.

### Deliverables

* [ ] Implement `cxxp info`.
* [ ] Define the initial resolved-project JSON output.
* [ ] Report project metadata.
* [ ] Report project root and manifest location.
* [ ] Report discovered project files.
* [ ] Report configured tools.
* [ ] Report configured build backend.
* [ ] Distinguish manifest input from resolved project information.
* [ ] Add tests for the `info` output.
* [ ] Document the `info` output sufficiently for programmatic consumers.

### Exit criteria

The following produces valid JSON:

```console
cxxp info
```

and the output contains enough information to perform useful project introspection without directly parsing `cxx.json`.

At minimum, the following should work:

```console
cxxp info | jq -r '.project.name'
cxxp info | jq -r '.project.version'
cxxp info | jq -r '.files.sources[]'
```

The output must be deterministic for an unchanged project.

---

## M2 — Formatting

Provide a standardized formatting workflow backed by `clang-format`.

### Deliverables

* [ ] Implement `cxxp format`.
* [ ] Implement formatting-file discovery.
* [ ] Support existing `.clang-format` configuration.
* [ ] Implement `cxxp format --check`.
* [ ] Define behavior for files that cannot be formatted.
* [ ] Define exit-status semantics.
* [ ] Add integration tests.
* [ ] Use `cxxp format --check` in the `cxxproject` development workflow.

### Exit criteria

For the `cxxproject` repository:

```console
cxxp format
```

formats all applicable files, and:

```console
cxxp format --check
```

returns success when the repository is correctly formatted and a nonzero exit status when a formatting violation exists.

No CMake invocation is required to perform formatting.

---

## M3 — Static Analysis

Provide a project-level static-analysis workflow backed by `clang-tidy`.

### Deliverables

* [ ] Implement `cxxp check`.
* [ ] Integrate with `compile_commands.json`.
* [ ] Define how the compilation database is obtained.
* [ ] Define behavior when no compilation database exists.
* [ ] Integrate existing `.clang-tidy` configuration.
* [ ] Define diagnostic severity and exit-status behavior.
* [ ] Establish an internal diagnostic representation.
* [ ] Add integration tests.

### Exit criteria

For the `cxxproject` repository:

```console
cxxp check
```

runs `clang-tidy` using the project's actual compilation information and returns a nonzero status when configured checks fail.

`cxxp check` must not require manually reconstructing compiler arguments in `cxx.json`.

---

## M4 — SARIF Diagnostics

Make diagnostics consumable by external tooling.

### Deliverables

* [ ] Define the initial normalized diagnostic model.
* [ ] Implement SARIF output.
* [ ] Implement:

```console
cxxp check --format sarif
```

* [ ] Support writing SARIF to a file.
* [ ] Preserve source locations and rule identifiers where available.
* [ ] Add tests validating generated SARIF.
* [ ] Validate generated SARIF against the applicable SARIF schema/specification.

### Exit criteria

A `cxxp check` invocation can produce a valid SARIF document that represents the diagnostics produced by `clang-tidy`.

The resulting file can be consumed by at least one external SARIF-compatible integration without requiring project-specific conversion.

---

## M5 — CMake Build Backend

Establish the frontend/backend model using CMake as the initial implementation.

### Deliverables

* [ ] Define the initial conceptual build-backend interface.
* [ ] Implement the CMake backend.
* [ ] Implement `cxxp build`.
* [ ] Define build-directory behavior.
* [ ] Define configuration behavior.
* [ ] Define build failure and exit-status semantics.
* [ ] Add integration tests.
* [ ] Build `cxxp` itself through the CMake backend.

### Exit criteria

From a clean checkout:

```console
cxxp build
```

successfully configures and builds the project through CMake.

The same project remains directly buildable using its existing CMake workflow.

The `cxx.json` manifest does not need to reproduce the project's CMake build graph.

---

## M6 — Configuration Layering

Establish composable configuration across scopes.

### Deliverables

* [ ] Define supported configuration scopes.
* [ ] Define configuration discovery rules.
* [ ] Define precedence.
* [ ] Define object merge semantics.
* [ ] Define array merge semantics.
* [ ] Define handling of conflicting values.
* [ ] Define command-line override behavior.
* [ ] Expose resolved configuration through `cxxp info`.
* [ ] Add tests covering configuration composition.

### Exit criteria

A project can inherit configuration from a broader scope and override or extend it locally using documented, deterministic rules.

For any effective configuration value, `cxxp info` can expose the resulting value and, where practical, its configuration source.

---

## M7 — C++ Language Requirements

Define a project-level representation of supported C++ language versions.

### Deliverables

* [ ] Determine whether language requirements belong in the core project model.
* [ ] Define minimum-version semantics.
* [ ] Define version-range semantics.
* [ ] Determine whether maximum versions should be supported.
* [ ] Determine how C and C++ requirements coexist.
* [ ] Consider compiler extensions and feature requirements.
* [ ] Consider interaction with future dependency/ABI requirements.
* [ ] Update the schema.
* [ ] Update `cxxp info`.

### Exit criteria

The project can unambiguously express at least:

1. no declared C++ version requirement,
2. a minimum supported C++ version, and
3. a range of supported C++ versions,

without implying that the project must be compiled using one particular standard version.

The semantics are documented with examples demonstrating the distinction between "requires" and "build using."

---

## M8 — Build Backend Extensibility

Replace the hardcoded CMake assumption with an extensible backend model.

### Deliverables

* [ ] Define the build-backend interface.
* [ ] Define backend discovery.
* [ ] Define backend versioning.
* [ ] Define backend capability discovery.
* [ ] Define failure behavior.
* [ ] Define security/trust considerations.
* [ ] Define third-party backend installation/distribution.
* [ ] Implement at least one backend without modifying the `cxxp` core.

### Exit criteria

A second build backend can be implemented and used by `cxxp` without changing the core project model or adding backend-specific logic to the main `cxxp` command implementation.

CMake continues to work through the same backend interface.

---

## M9 — Project Model Expansion

Expand `cxx.json` only where demonstrated use cases justify additional project concepts.

Potential areas include:

* [ ] targets
* [ ] libraries
* [ ] executables
* [ ] test programs
* [ ] generated files
* [ ] include paths
* [ ] dependencies
* [ ] project resources
* [ ] C and C++ language requirements
* [ ] platform requirements

This milestone should not be treated as a commitment to add all of these concepts.

### Exit criteria

Each addition to the core project model has:

1. a demonstrated consumer,
2. documented semantics,
3. schema validation,
4. `cxxp info` representation, and
5. at least one integration test.

---

# Cross-cutting design work

Some work does not belong cleanly to a single implementation milestone.

## Schema versioning

* [ ] Define schema-version compatibility rules.
* [ ] Define behavior when `cxxp` encounters a newer schema version.
* [ ] Define migration expectations.
* [ ] Establish the first stable schema version only after sufficient implementation experience.

## Tool integration model

* [ ] Determine how tool integrations are discovered.
* [ ] Define the distinction between project-level tool configuration and native tool configuration.
* [ ] Determine how multiple tools serving the same purpose coexist.
* [ ] Establish conventions for tool-specific output and capabilities.

## Diagnostic model

* [ ] Define the normalized diagnostic representation.
* [ ] Define diagnostic severity semantics.
* [ ] Define rule identity.
* [ ] Define source locations and related locations.
* [ ] Define fix/patch representation.
* [ ] Determine how compiler diagnostics fit alongside analyzer diagnostics.

## Agent interface

* [ ] Evaluate whether `cxxp info` provides sufficient project introspection.
* [ ] Identify information agents commonly need that is not represented.
* [ ] Define stable machine-readable interfaces separately from human CLI presentation.
* [ ] Evaluate whether additional read-only introspection commands are warranted.

---

# Open Questions

These are active design questions rather than unresolved implementation bugs. They should be answered through design work and experimentation.

## Build backends

* What is the minimal useful backend interface?
* How should third-party backends be discovered?
* How should backend versions and capabilities be represented?
* How should backend execution be trusted?

See M5 and M8.

## Configuration

* What configuration scopes should exist?
* How should configuration be merged?
* How should users inspect the origin of resolved configuration?

See M6.

## Language requirements

* What is the correct semantic model for C/C++ version requirements?
* Should requirements be expressed as ranges?
* Should maximum versions be allowed?
* How should language requirements interact with ABI and dependencies?

See M7.

## Project model

* Which concepts belong in `cxx.json`?
* When does representing build targets become useful rather than duplicative?
* How much source discovery should be standardized?

See M9.

## Diagnostics

* How should project-level diagnostic requirements map onto compiler-specific capabilities?
* Should compiler warnings become first-class project properties?
* How should unsupported diagnostics be handled?
* How should different analyzers contribute to a single project check?

See M3 and the diagnostic-model work.

---

# Guiding Principle

The roadmap should favor **useful standardization over comprehensive standardization**.

A feature should generally enter the common project model only when there is a concrete benefit to having that information standardized across tools.

In particular:

> **Do not encode build-system implementation details in `cxx.json` merely because they exist in a build system.**

The success of the project is not measured by how much of CMake, Meson, Bazel, or another build system can be represented in JSON.

It is measured by whether a project can expose useful, portable semantics to developers and tools while allowing existing build systems to remain responsible for the mechanics of building the software.

