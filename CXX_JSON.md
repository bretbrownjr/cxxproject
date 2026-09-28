# cxx.json reference

The `cxx.json` file contains a project's name and version, along with the schema
revision that identifies the file format. A minimal example looks like this:

```json
{
  "schemaVersion": 1,
  "project": {
    "name": "example",
    "version": "0.1"
  }
}
```

Schema revision 1 has four required fields:

| Field | Type | Description |
| --- | --- | --- |
| `schemaVersion` | Integer | File format revision. Currently, only `1` is supported. |
| `project` | Object | Project metadata containing `name` and `version`. |
| `project.name` | String | Project name; cannot be empty. |
| `project.version` | String | Project version; cannot be empty. No particular version format is required. |

The file contains a JSON object. Extra fields, either at the top level or inside
`project`, cause validation errors, which helps catch misspelled field names.

The schema revision uses an integer representation: `1` is accepted, but `1.0`
is not.

The [JSON Schema (Draft 7)](schemas/cxx-json-v1.schema.json) provides the formal
field definitions. `cxxp info` finds the nearest `cxx.json`, rejects duplicate
object keys, reads it, and validates it before reporting project information.
See the [command reference](CLI.md) for discovery and error behavior.

## Formatting backend

The optional `formatting` object declares formatter requirements:

```json
{
  "schemaVersion": 1,
  "project": { "name": "example", "version": "0.1" },
  "formatting": { "backend": "clang-format" }
}
```

`formatting.backend` defaults to `clang-format`, currently the only accepted
value. An empty `formatting` object also uses that default. Other types,
unsupported backends, and unknown formatting fields are rejected.

Loading a project records this requirement without running tools or reading
a lockfile. The exact selected release belongs in
[cxx.lock.json](CXX_LOCK_JSON.md), not the manifest. The executable path is
derived from the local environment of the user, for instance the `PATH`
environment variable.
