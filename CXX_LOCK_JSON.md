# cxx.lock.json reference (planned)

The [JSON Schema (Draft 7)](schemas/cxx-lock-json-v1.schema.json) defines the
planned format. Runtime lockfile validation and maintenance are not implemented
yet. Project dependency resolution remains deferred.

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

The shared lock is `cxx.lock.json` beside `cxx.json`. Planned schema revision 1
defines these fields:

| Field | Type | Description |
| --- | --- | --- |
| `schemaVersion` | Integer | Lockfile format revision, independent of the manifest revision. Initially `1`. |
| `projectDependencies` | Object | Reserved for dependencies used to produce the project's binary. Initially empty; library resolution is deferred. |
| `developmentTools` | Object | Tool records keyed by identifier, such as `clang-format`. Missing required records follow the initialization policy below. |
| `developmentTools.<tool>` | Object | A selected tool's record, containing its `version`. |
| `developmentTools.<tool>.version` | String | Exact upstream release, including the patch version, such as `22.1.8`. |

## Version identity

* Versions identify exact upstream releases, including patch versions.
* Vendor suffixes are excluded from version identity but retained in diagnostics.
* Unrecognized or ambiguous version identities cause an error.
* Initial formatter validation targets upstream release `22.1.8`.
* Broader version support requires validation evidence.

## Lock contents and output

* All three top-level fields are required; unknown top-level fields are rejected.
* Tool identifiers and version strings must be nonempty.
* Tool records require `version` and reject unknown fields.
* An empty `developmentTools` object is valid; required tool records depend on
  the operation and follow the initialization policy below.
* The project-dependency category is reserved for build dependency resolution.
* Its contents are unconstrained by this schema so existing records can be
  preserved without implementing dependency resolution.
* Formatting preserves project-dependency records and unrelated tool records.
* Lock output is deterministic.
  * No timestamps.
  * No local executable paths.

## Initialization and updates

* Duplicate-key detection and tool-specific release validation occur outside the
  JSON Schema.
* Formatting and lock maintenance share tool identification and validation.
* Lock writes are atomic.
* Missing required records are initialized from installed tools.
* `--locked` forbids initialization of missing records.
* Existing required records are validated before records are added.
* A version mismatch causes an error without source or lock edits unless the
  tool is explicitly targeted for update.
* Invalid locks cause errors rather than being silently replaced.
* Failed tool identification preserves the old lock.
* Failed lock writes preserve the old lock.

## Help and inspection

Help and `info` do not:
  * Execute tools.
  * Require a lock.

See the [command reference](CLI.md#cxxp-format)
for automatic initialization, explicit updates, and recovery guidance.
