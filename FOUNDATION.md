# Foundation action plan

The first [roadmap milestone](ROADMAP.md#first-proof-of-concept) is a small working
`cxxp` that can find and validate a project. The plan below starts with a runnable
program and iterates toward that goal. Schema details and the proposed `validate`
command are still open to discussion.

## 1. Get a program running

Complete. See [BUILDING.md](BUILDING.md) to build and test the runnable scaffold.

## 2. Agree on a small manifest

The initial `cxx.json` needs only a project name, project version, and schema-version
identifier. A versioned JSON Schema will define which fields are required, their
allowed types, and what happens with unknown fields or unsupported schema versions.
Project versions can remain metadata without requiring a particular versioning
scheme.

Bundle the schema with the tool so validation works offline. Include examples of
valid and invalid manifests in the tests, and give this repository its own
`cxx.json`. The agreed manifest rules belong in
[DESIGN.md](DESIGN.md#project-interface).

## 3. Find and read the project

Starting in the current directory, `cxxp` will look upward for the nearest
`cxx.json`. The directory containing it is the project root. If that manifest is
invalid, the tool should report the problem rather than continue looking for a
parent project.

Discovery and manifest loading should be usable independently of the command-line
interface. Together they provide the project root, manifest path, name, and version.
Missing manifests, unreadable files, malformed JSON, and schema violations need
clear errors, including the affected path where available.

## 4. Try the whole workflow

A proposed `cxxp validate` command would bring these pieces together: find the
project, validate its manifest, and report its name and version. Failure would
return a nonzero exit status. Resolved JSON output can follow in the introspection
milestone.

Tests should exercise the executable from project roots and nested directories,
including nested projects, missing manifests, invalid JSON, invalid fields, and
unsupported schemas. They should also check help and version output, error
messages, and exit status. CI can run the same build and tests documented for
contributors.

## When Foundation is done

A fresh checkout can be built and tested using the contributor instructions.
The executable can validate this repository and the test projects without network
access. At that point, the README and roadmap should reflect the working tool.
