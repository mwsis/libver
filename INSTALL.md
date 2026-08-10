# **libver** - Installation and Use <!-- omit in toc -->

**libver** is a C-core tool-suite: a static library with a C-API, plus thin
CLI frontends. The primary build path is **CMake**.


## Table of Contents <!-- omit in toc -->

- [CMake](#cmake)


## CMake

1. Obtain the **libver** sources.

2. Prepare the CMake configuration:

   ```bash
   $ ./prepare_cmake.sh -v
   ```

3. Build:

   ```bash
   $ ./build_cmake.sh
   ```

Useful optional flags for **prepare_cmake.sh**:

* `--disable-cli` — omit the shared **libver** CLI (`BUILD_CLI=OFF`);
* `--disable-frontends` — omit ecosystem frontends such as **cargo-libver** (`BUILD_FRONTENDS=OFF`);
* `--disable-testing` / `-T` — omit tests (`BUILD_TESTING=OFF`);
* `--run-make` / `-m` — build immediately after configure;
* `--debug-configuration` / `-d` — Debug build type;

Execute `$ ./prepare_cmake.sh --help` for the full set of options.


## Running tests

Configure and build (default build dir **_build/**):

```bash
$ ./prepare_cmake.sh -m
```

Then either:

```bash
$ ./ctest_cmake.sh            # CTest-registered tests
$ ./run_all_unit_tests.sh     # discovers test.unit.* / test.component.* binaries
$ ./run_all_scratch_tests.sh  # discovers test.scratch.* / test.performance.* binaries
```

Useful flags:

* `--no-make` / `-M` — do not rebuild before running;
* `--list-only` / `-l` — list matching programs only;
* `--unit-only` / `--component-only` — filter (**run_all_unit_tests.sh**);


<!-- ########################### end of file ########################### -->

