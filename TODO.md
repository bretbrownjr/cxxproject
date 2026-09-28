# Follow-up work

- Consult Conan upstream about making `CPSDeps` add its generated search path to
  `CMAKE_PREFIX_PATH` automatically. This is an experimental feature, so report
  the project workflow and provide feedback upstream.
- Consider a cxxp feature for custom regular-expression-based linters, for coding
  standards that can be expressed reliably with regular expressions.
- Add `SeparateDefinitionBlocks: Always` to `.clang-format` and reformat the C++
  sources and headers to separate definitions with blank lines.
- Support SARIF output for all modes. Use failed invocations for system errors and
  misconfigurations. Use results for warnings and errors.
- Move all intersting behavior from `cxxp/main.cxx`. That file should be almost
  empty. All interesting behavior should exist in the `cxxp` implementation library
  that can be used in various test drivers.
- Find a better name for the `cxxp` implementation library.
- Stop throwing exceptions based on user input or environments. These are normal
  situations to encounter and are not exceptional. Invalid inputs or environments
  should normally result in diagnostics to the users, eventually in SARIF form. Data
  structures to support encoding in SARIF results are required.
- Add clang-tidy checks to the repo enable one that always requires curly braces
  in control structures to avoid bugs like the "goto fail" bug.
