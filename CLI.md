# Command-line reference

`cxxp` provides project information, source formatting, and development tool
locking. For workflow examples, see the [usage guide](USAGE.md).

## Top-level usage

```text
cxxp [--help | --version]
cxxp info [--help]
cxxp format [--check] [--locked]
cxxp lock [--update-tool clang-format]
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

## `cxxp format`

```text
cxxp format [--check] [--locked]
cxxp format --help
```

`format` formats the project's source and header files in place.

Run it from the project root or any subdirectory. It uses the same
[discovery rules](#discovery) as `info` and requires no particular context such
as build files or a compilation database.

Only `clang-format` is supported at this time. The command is supported on
Linux and finds the executable named `clang-format` on your `PATH`. Install
it before running `format`; `cxxp` does not install tools or automatically
select version-suffixed executables. There is no executable-path override
at this time.

### Options

| Option | Effect |
| --- | --- |
| `--check` | Check formatting without changing source files. Exit with `1` if formatting is needed. May initialize missing lock records. |
| `--locked` | Require a valid lockfile containing the required tool records. Never write lock metadata. Can be used with or without `--check`. |
| `--help` | Print command usage without requiring a project, lockfile, or installed formatter. Must be used alone. |

Use `cxxp format --check --locked` to check formatting without changing sources
or lock metadata. Options may appear in either order but may not repeat.
Explicit file paths, style overrides, and other options are not accepted.

### Configuration and version selection

* `clang-format` uses its native configuration lookup, including parent directories.
* Without a configuration file, `clang-format` uses LLVM style.
* Native `clang-format` ignore rules can exclude files from the discovered inventory.
* The configuration paths reported by `info` do not determine which style applies
  to each source file.
* Missing lock records are initialized from the installed formatter unless
  `--locked` is used.
* The installed release must match an existing locked release. A mismatch leaves
  both sources and the lock unchanged.
* A project with no matching files still requires tool and lock validation, then
  succeeds without invoking a formatting operation.

See the [manifest reference](CXX_JSON.md#formatting-backend) for backend
selection and the [lockfile reference](CXX_LOCK_JSON.md) for lock fields.

### Output and failures

* Successful formatting and clean checks exit with `0`.
* Formatting differences in check mode and all errors exit with `1`.
* Help writes to stdout. Otherwise, stdout is empty and diagnostics go to stderr.
* Formatting stops at the first failed file. Earlier edits remain.
* Checking continues after individual file failures and exits with `1` if any
  file fails. Failure to launch `clang-format` stops either mode immediately.
* A lock initialized before a formatting failure remains in place.

## `cxxp lock`

```text
cxxp lock [--update-tool clang-format]
cxxp lock --help
```

`lock` records installed development tool versions in `cxx.lock.json` beside
`cxx.json`. Run it from the project root or a subdirectory. It initializes missing
records for the project's selected tools and validates existing selections.
Only clang-format is supported at this time.

### Options

| Option | Effect |
| --- | --- |
| `--update-tool clang-format` | Replace the formatter's selected release with its installed release, or create its record if missing. The installed release may be older than the previous selection. |
| `--help` | Print command usage without requiring a project, lockfile, or installed tool. Must be used alone. |

`--update-tool` accepts one tool identifier per invocation and may not repeat.
`--check`, `--locked`, positional arguments, and other tool identifiers are not
accepted.

An explicit update preserves unrelated tool and project-dependency records.
Locking never changes sources or installs tools. Run `cxxp format` after updating
the formatter's record to apply the selected release.

Successful locking exits with `0`; errors exit with `1`. Help writes to stdout.
Otherwise, stdout is empty and diagnostics go to stderr. Invalid lockfiles are
not overwritten. Tool identification or lock-write failures preserve the old lock.

### Resolving errors

Both `format` and `lock` reject invalid arguments before looking for a project or
running tools. For tool and lock errors:

| Error | Action |
| --- | --- |
| Installed release differs from the lock | Select the locked release on your `PATH`, or run `cxxp lock --update-tool clang-format` to deliberately select the installed release, then run `cxxp format`. The diagnostic includes both releases and the executable path. |
| Missing lock or required record with `--locked` | Run `cxxp lock`, review its changes, and retry `cxxp format --check --locked`. |
| Formatter missing or its release cannot be identified | Install or select a compatible clang-format release on your `PATH`, matching the lock when present, then retry. |
| Invalid manifest or lockfile | Correct the reported field or restore a valid file, then retry. |
| Invalid formatter configuration or source file | Fix the reported file and rerun formatting. Updating the lock does not fix these errors. |
