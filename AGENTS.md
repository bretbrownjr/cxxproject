# Repository instructions

## Documentation roles

Keep documentation short enough for a new contributor to read and a maintainer
to keep current. Each file owns a distinct category of information:

| File | Purpose |
| --- | --- |
| [README.md](README.md) | Explain the project, its motivation, and current maturity; point readers to other documents. |
| [DESIGN.md](DESIGN.md) | Record design principles, boundaries, consequential decisions, and open design questions. |
| [CXX_JSON.md](CXX_JSON.md) | Explain the manifest format, supported fields, and validation constraints for users. |
| [ROADMAP.md](ROADMAP.md) | Describe planned capabilities and their order; distinguish immediate work from deferred ideas. |
| [FOUNDATION.md](FOUNDATION.md) | Break the Foundation milestone into actionable steps and completion checks; keep milestone summaries in ROADMAP.md. |
| [CONTRIBUTING.md](CONTRIBUTING.md) | Explain how to contribute and validate changes, with commands only when they actually work. |
| [DEVELOPMENT.md](DEVELOPMENT.md) | Document development tools, formatting, and build and test commands. |
| [TODO.md](TODO.md) | Track concrete follow-up issues and upstream questions. |
| AGENTS.md | Guide agent editing and review workflows; keep project explanations in the files above. |

## Audience and voice

Only explicitly agentic instruction files, such as AGENTS.md, should address
agents or prescribe agent workflows. Write project documentation for human readers
in natural language, including implementation plans. Explain the work and expected
behavior rather than presenting a prompt or a sequence of agent directives.

Keep any additional agent-specific guidance in AGENTS.md or a separate, clearly
identified agent instruction file. Technical requirements and contributor guidance
still belong in their owning project documents.

## Editing workflow

Development tools, including CMake, Ninja, clang-tidy, clang-format, and just,
are installed under `/opt/uv/.venv/bin`. Add that directory to PATH for validation.
Do not download development tools; ask the user if a required tool is missing.

Run `just ci` before handing off changes, using the appropriate `toolchain` and
`build_dir` settings for the environment. It formats files in place, then builds
and tests; review the resulting diff. See [DEVELOPMENT.md](DEVELOPMENT.md) for setup and
command options. If validation cannot run, report the blocker.

Always use angle brackets for header includes, including project headers. Prefix
project header paths with `cxxp/` and expose them through CMake header file sets.
Put code in namespace `cxxp` wherever possible, including unnamed namespaces.
Keep the global `main` function minimal and delegate its logic to `cxxp`.
Use include guards with project-prefixed macro names instead of `#pragma once`.

* **Placement:** Put substantive information in its owning file and link to it elsewhere. A brief
  orientation is fine; repeated explanations and checklists are not.
* **Scope:** Update only documents affected by a change. Do not expand all files for symmetry.
* **Accuracy:** Verify status claims, commands, and examples against the repository. Distinguish
  implemented behavior, planned work, and provisional decisions.
* **Maintenance:** Prefer replacing stale text to appending qualifications or progress history.
* **Brevity:** Preserve useful rationale, constraints, and unresolved questions when shortening
  text. Omit boilerplate, speculative detail, and exhaustive task inventories.
  Add detail only when it helps a reader understand, decide, or take action.

## Project version

Every PR must bump the version in the root CMake `project()` declaration,
including documentation-only PRs. During development, include a version increase
relative to the PR's base branch before submitting the change. During review,
verify that increase and report a missing bump as an issue. Keep executable
version output derived from the project version rather than maintaining a second
version string.

## Review workflow

Apply these checks to documentation changes and to code changes that may make
documentation stale:

* **Placement:** Does information belong in this file, or should it move to its owner with a link?
* **Duplication:** Does another file already explain it? Can the duplication be removed?
* **Audience:** Is project documentation written for humans, with agent-directed
  wording confined to explicitly agentic instruction files?
* **Accuracy:** Do status claims and instructions match the implementation? Are plans clearly
  identified, and do examples and links work?
* **Consistency:** Has the change left conflicting or stale guidance elsewhere?
* **Brevity:** Does added text justify its reading and maintenance cost at the project's current
  maturity? Can it be shortened without losing useful meaning?

**Review output:** Report concrete issues with file locations and suggested fixes. Do not request
extra sections or detail merely to make documentation look comprehensive.
