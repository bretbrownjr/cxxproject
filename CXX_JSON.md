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
