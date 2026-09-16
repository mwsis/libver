# **libver** - Installation and Use <!-- omit in toc -->

**libver** is a C-core tool-suite: a static library with a C-API, plus thin
CLI frontends. The primary build path is **CMake**.

The C core (`libver_find`) discovers **Cargo.toml** and **build.zig.zon**
versions in a given directory (not recursively). The shared **libver** CLI
probes every known scheme in that order; **cargo-libver** probes
**Cargo.toml** only. Obtain third-party dependencies from
[REQUISITES.md](./REQUISITES.md) — this file does not repeat those install
novels.


## Table of Contents <!-- omit in toc -->

- [CMake](#cmake)
	- [Dependencies](#dependencies)
- [Running the CLIs](#running-the-clis)
- [Running tests](#running-tests)


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


### Dependencies

Full obtain / build / install steps for every third-party dependency (including **tomlc17**) are in [REQUISITES.md](./REQUISITES.md).

Summary:

* Always: **Diagnosticism**, **recls**, **STLSoft**, **tomlc17**;
* When building CLIs / frontends (`BUILD_CLI` / `BUILD_FRONTENDS`): **CLASP**, **collect-c**, **cstring**, **sistools-common-c**;
* When testing (`BUILD_TESTING`): **xTests**;

Hint local build trees / install prefixes via **CMAKE_PREFIX_PATH** or:

* `COLLECT_C` — e.g. `$HOME/dev/synesissoftware/forks/freelibs/collect/collect-c/_build`
* `DIAGNOSTICISM` — e.g. a local **Diagnosticism** `_build` or install prefix
* `PANTHEIOS` — for later logging; enable required locate with `-DLIBVER_WITH_PANTHEIOS=ON`
* `RECLS` — e.g. `$HOME/dev/synesissoftware/forks/freelibs/recls/recls/_build`
* `SISTOOLS_COMMON_C` — e.g. `$HOME/dev/sistools/sistools-common-c/_build`
* `STLSOFT` — unpacked tree (include path `${STLSOFT}/include`); see **REQUISITES.md**
* `TOMLC17_INCLUDE_DIR` / `TOMLC17_LIBRARY` — if **tomlc17** is not on the default search path

After build, CLIs:

```bash
$ ./_build/cli/libver/libver --help
$ ./_build/frontends/cargo-libver/cargo-libver --version
```


## Running the CLIs

Omitted `<directory>` is the process current working directory (resolved,
then passed to `libver_find`). Detection is **not** recursive.

```bash
$ ./_build/cli/libver/libver core/test/fixtures/cargo-only
scheme:  cargo
version: 1.2.3
source:  core/test/fixtures/cargo-only/Cargo.toml

$ ./_build/cli/libver/libver core/test/fixtures/zig-only
scheme:  zig
version: 0.4.5
source:  core/test/fixtures/zig-only/build.zig.zon

$ ./_build/cli/libver/libver core/test/fixtures/empty
libver: no recognised project version: '.../empty'
```

The last command exits **1** (`LIBVER_RC_NO_MATCH`). Other positive
`LIBVER_RC_*` values are used as the process status; usage errors also
exit **1**.

**cargo-libver** is the Cargo-subcommand name. Invoke it directly, or put
its directory on `PATH` and run `cargo libver` (Cargo looks up
`cargo-libver`):

```bash
$ ./_build/frontends/cargo-libver/cargo-libver core/test/fixtures/cargo-only
scheme:  cargo
version: 1.2.3
source:  core/test/fixtures/cargo-only/Cargo.toml

$ PATH="$PWD/_build/frontends/cargo-libver:$PATH" cargo libver
```

Cargo invokes **cargo-libver** with the subcommand name `libver` as a
leading argument; the frontend skips that token. `cargo libver` needs a
Cargo tree as cwd (or pass `<directory>` after the subcommand). It does
not probe **build.zig.zon**. If `cargo libver` is not picked up, use the
standalone **cargo-libver** path above.


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
