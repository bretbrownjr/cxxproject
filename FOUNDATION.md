# Foundation action plan

The first [roadmap milestone](ROADMAP.md#first-proof-of-concept) is a small working
`cxxp` that can find and validate a project. The plan below starts with a runnable
program and iterates toward that goal. The proposed `validate` command is still open to discussion.

## 1. Get a program running

Complete. See [DEVELOPMENT.md](DEVELOPMENT.md) to build and test the runnable scaffold.

## 2. Agree on a small `cxx.json` format

Complete. The [`cxx.json` contract](CXX_JSON.md) and bundled schema
are implemented by a validator independent of the CLI.

## 3. Find and read the project

Complete as part of Introspection. `cxxp info` searches upward from the current
directory for the nearest `cxx.json`, rejects duplicate object keys, validates
the file, and reports errors without falling back to a parent project. See
the [command reference](CLI.md).

## 4. Try the whole workflow

Complete through `cxxp info`, which resolves the project and emits structured
JSON. A standalone `validate` command is not part of the current plan.

Tests should exercise the executable from project roots and nested directories,
including nested projects, missing `cxx.json` files, invalid JSON, invalid fields, and
unsupported schemas. They should also check help and version output, error
messages, and exit status. CI can run the same build and tests documented for
contributors.

## When Foundation is done

A fresh checkout can be built and tested using the contributor instructions.
The executable can validate this repository and the test projects without network
access. At that point, the README and roadmap should reflect the working tool.
