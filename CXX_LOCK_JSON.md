# cxx.lock.json reference

`cxx.lock.json` records the project's selected development tool versions. It
lives beside `cxx.json`; the [JSON Schema (Draft 7)](schemas/cxx-lock-json-v1.schema.json)
defines its format.

The library validates, loads, updates, and atomically writes lockfiles on Linux.
Command integration and automatic initialization remain planned. Project
dependency resolution is deferred.

A minimal example locks clang-format to release `22.1.8`:

```json
{
  "schemaVersion": 1,
  "projectDependencies": {},
  "developmentTools": {
    "clang-format": {
      "version": "22.1.8"
    }
  }
}
```

## Fields and validation

Schema revision 1 defines these fields:

| Field | Type | Description |
| --- | --- | --- |
| `schemaVersion` | Integer | Lockfile format revision, independent of the manifest revision. Currently `1`; `1.0` is not accepted. |
| `projectDependencies` | Object | Reserved for dependencies used to produce the project's binary. Use an empty object until library resolution is supported. Existing contents are preserved without interpretation. |
| `developmentTools` | Object | Tool records keyed by identifier, such as `clang-format`. May be empty; required records depend on the operation. |
| `developmentTools.<tool>` | Object | A selected tool's record, containing its `version`. |
| `developmentTools.<tool>.version` | String | Exact upstream release, including the patch version, such as `22.1.8`. |

All three top-level fields are required. Unknown top-level fields and unknown
fields in tool records are rejected. Tool identifiers and version strings must
be nonempty.

The library also rejects malformed JSON, duplicate keys at any depth, and
unsupported schema revisions. Duplicate-key detection and tool-specific release
validation supplement the JSON Schema. An empty tool map is valid, but an
operation can require particular tool records.

## Version identity and support

For clang-format, a stored version has three numeric components, such as
`22.1.8`, and identifies an exact upstream release. Vendor information is excluded
from the release identity but retained in tool diagnostics. Unrecognized or
ambiguous tool version output causes an error. Other tools' version strings are
preserved without interpreting them beyond requiring a nonempty string.

A valid lock record does not imply that the adapter supports that release.
The built-in clang-format adapter currently supports only `22.1.8`; broader
support requires validation evidence. Lock validation accepts other releases,
such as `23.0.0`, so their records can be preserved even when the current adapter
cannot use them.

## Updates and write guarantees

Updating one tool preserves all other tool records and project-dependency
records. Output is deterministic and contains neither timestamps nor local
executable paths.

Updates replace the lockfile atomically. If writing fails before replacement,
the existing lock remains unchanged and the temporary file is removed.

On Linux, the library creates an exclusive temporary file in the same directory,
writes, flushes, and closes it, then renames it over the destination. This does
not coordinate concurrent writers or guarantee that the replacement survives a
power loss.

## Initialization and updates (planned command policy)

Formatting and lock maintenance will share tool identification and validation.
Missing required records will be initialized from installed tools unless
`--locked` forbids initialization. Existing required records must pass validation
before any missing records are added.

A version mismatch will cause an error without source or lock edits unless the
tool is explicitly targeted for update. Invalid locks will cause errors rather
than being silently replaced, and failed tool identification will preserve the
old lock.

Help and `info` do not execute tools or require a lock. See the
[command reference](CLI.md#cxxp-format) for planned automatic initialization,
explicit updates, and recovery guidance.
