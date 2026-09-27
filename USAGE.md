# Using cxxp

Build the project as described in [Development](DEVELOPMENT.md), then run
`cxxp info` from a project directory to inspect its metadata and discovered files:

```sh
build/src/cxxp info
```

The command searches the current directory and its parents for the nearest
`cxx.json`, so it also works from a subdirectory. It does not configure or build
the project. Save the JSON output for another tool with shell redirection:

```sh
build/src/cxxp info > project-info.json
```

If the command cannot find `cxx.json`, run it from within a project that has
one. If it reports an invalid file, fix the nearest `cxx.json`; `cxxp` will not
skip it in favor of a parent project.

See the [command-line reference](CLI.md) for the output fields, file discovery
rules, and exit behavior.
