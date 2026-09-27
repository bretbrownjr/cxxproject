# Command-line reference

`cxxp` currently supports help, version output, and the `info` command. For
workflow examples, see the [usage guide](USAGE.md).

## Top-level usage

```text
cxxp [--help | --version]
cxxp info [--help]
```

With no arguments or with `--help`, `cxxp` prints general usage. `--version`
prints the project version. Unknown arguments and multiple top-level options
exit with status `1` and print a diagnostic to stderr.

## `cxxp info`

```text
cxxp info [--help]
```

`info` writes a JSON document to stdout. `info --help` prints command usage and
does not require a project.

### Output

`outputVersion` is the integer `1`, independent of the `cxx.json` file's
`schemaVersion`. The document contains these fields:

| Field | Type | Meaning |
| --- | --- | --- |
| `outputVersion` | Integer | Version of the `info` output format. |
| `project.name` | String | Project name from `cxx.json`. |
| `project.version` | String | Project version from `cxx.json`. |
| `root` | String | Absolute canonical project root. |
| `projectFile` | String | Path to `cxx.json` relative to the root. |
| `files.sources` | Array of strings | Discovered source paths relative to the root. |
| `files.headers` | Array of strings | Discovered header paths relative to the root. |
| `tools.clang-format.configurationFiles` | Array of strings | Discovered `.clang-format` and `_clang-format` paths. |
| `tools.clang-tidy.configurationFiles` | Array of strings | Discovered `.clang-tidy` paths. |
| `build.backend` | String or null | `cmake` when the root contains `CMakeLists.txt`; otherwise `null`. |
| `build.entryPoint` | String or null | `CMakeLists.txt` when the root contains it; otherwise `null`. |

Also note:

* Paths use `/` separators
* Arrays are sorted lexicographically and are present when empty
* JSON is formatted with human-friendly newlines and indentation
* Invoking `info` from the root or a nested directory produces identical
  output for the same project
* The output contains no timestamps or tool versions

### Discovery

`info` starts in the current directory and walks toward the filesystem root until
it finds a `cxx.json`. That file defines the project root. If the nearest
`cxx.json` is invalid, `info` reports the error instead of trying a parent project.

For example, imagine both `cxx.json` files below are valid:

```text
$ tree -a /work/demo
/work/demo
├── .clang-format
├── CMakeLists.txt
├── build
│   └── generated.cxx
├── cxx.json
├── include
│   └── api.hxx
├── src
│   └── main.cxx
├── tools
│   └── _clang-format
└── vendor
    ├── cxx.json
    └── library.cxx
```

Running `cxxp info` from `/work/demo/src` selects `/work/demo/cxx.json` and
reports:

* `files.sources`: `["src/main.cxx"]`
* `files.headers`: `["include/api.hxx"]`
* `tools.clang-format.configurationFiles`:
  `[".clang-format", "tools/_clang-format"]`
* `build.backend`: `"cmake"`, because the root has `CMakeLists.txt`

The generated file under `build` and the nested project under `vendor` are
left out. Running the command from `vendor` selects `vendor/cxx.json` as a
separate project.

Within the selected project, discovery recognizes:

* Source files ending in `.c`, `.cc`, `.cpp`, or `.cxx`.
* Header files ending in `.h`, `.hh`, `.hpp`, or `.hxx`.
* `.clang-format`, `_clang-format`, and `.clang-tidy` configuration files.

Names and extensions are case-sensitive. Discovery skips these directory trees:

* Directories named `.git`, `.hg`, `.svn`, or `build`, and directories whose
  names begin with `cmake-build-`.
* Directories containing `CMakeCache.txt` or their own `cxx.json`.
* Symlink entries, including links to files and directories.

Configuration files follow the same discovery rules as source files. Files
outside the project are not inspected. Git ignore rules do not affect the
results, and the file inventory does not claim to list the build system's inputs.

### Exit status and errors

`cxxp` exits with status `0` when it:

* Prints general help, `info` help, or the version.
* Successfully reports project information.

It exits with status `1` when it encounters:

* Invalid command-line arguments.
* No `cxx.json` in the current directory or its ancestors.
* A `cxx.json` file that cannot be read, contains malformed JSON or duplicate
  object keys, or fails schema validation.
* A file-discovery error.

Errors write a diagnostic to stderr. A failed `info` command leaves stdout empty.
