# **libver** - Requisites <!-- omit in toc -->


## Table of Contents <!-- omit in toc -->

- [Introduction](#introduction)
- [Required to build tools](#required-to-build-tools)
	- [Installation by CMake](#installation-by-cmake)
		- [CLASP](#clasp)
		- [Diagnosticism](#diagnosticism)
		- [Pantheios - optional (locate-only for now)](#pantheios---optional-locate-only-for-now)
		- [STLSoft](#stlsoft)
		- [collect-c](#collect-c)
		- [cstring](#cstring)
		- [recls](#recls)
		- [sistools-common-c](#sistools-common-c)
	- [Installation by other means](#installation-by-other-means)
		- [tomlc17](#tomlc17)
- [Required to build tests](#required-to-build-tests)
	- [Installation by CMake](#installation-by-cmake-1)
		- [xTests](#xtests)
	- [Installation by other means](#installation-by-other-means-1)
- [Further help](#further-help)


## Introduction

**libver** is a C-core tool-suite. Third-party dependencies fall into two groups: those needed to build the library and tools (CLI / frontends), and those needed only when building tests.

After installing (or pointing at a local `_build` tree), ensure **CMAKE_PREFIX_PATH** and/or the hint variables named in [INSTALL.md](./INSTALL.md) can see CMake packages. For **tomlc17**, use the default search path or `TOMLC17_INCLUDE_DIR` / `TOMLC17_LIBRARY`.

Minimum versions requested by this project's **CMakeLists.txt** (as of writing): **CLASP** 0.15; **Diagnosticism** 0.3; **STLSoft** 1.11; **collect-c** 0.1; **cstring** 4; **recls** 1.10; **sistools-common-c** 0.1; **xTests** 0.26; **Pantheios** 1.0 (when located).


## Required to build tools

Always:

* [**Diagnosticism**](https://github.com/synesissoftware/Diagnosticism) — diagnostics helpers (e.g. version-string formatting);
* [**STLSoft**](https://github.com/synesissoftware/STLSoft-1.11) — discrimination / compatibility / utility headers (also pulled in by several Synesis dependents);
* [**recls**](https://github.com/synesissoftware/recls) — recursive filesystem search (scratch **test.scratch.python2**; expected again in core discovery);
* [**tomlc17**](https://github.com/cktan/tomlc17) — TOML parser for **Cargo.toml** probing (core Cargo backend; scratch **test.scratch.rust**);

When building the shared CLI and/or ecosystem frontends (`BUILD_CLI` / `BUILD_FRONTENDS`):

* [**CLASP**](https://github.com/synesissoftware/CLASP) — command-line argument parsing;
* [**collect-c**](https://github.com/synesissoftware/collect-c) — C collections used by the CLI stack;
* [**cstring**](https://github.com/synesissoftware/cstring) — resizeable C strings;
* [**sistools-common-c**](https://github.com/mwsis/sistools-common-c) — shared sistools C CLI helpers;

Optional (CMake locate only today; not linked until logging / **libCLImate** land):

* [**Pantheios**](https://github.com/synesissoftware/Pantheios) — logging; enable a required locate with `-DLIBVER_WITH_PANTHEIOS=ON`;

> **NOTE**: you may omit the CLI stack if both `BUILD_CLI` and `BUILD_FRONTENDS` are OFF. **recls** and **tomlc17** are currently required at configure time regardless.


### Installation by CMake

The libraries in this subsection install via their **prepare_cmake.sh** / **CMake** flow.


#### CLASP

The **CLASP** library provides command-line argument parsing, including parsing of arguments into _flags_, _options_, and _variables_, as well as utility facilities (such as pro-forma `--help` / `--version` output).

**CLASP** is obtained from **https://github.com/synesissoftware/CLASP**, and it provides the means to install via **CMake**, as in the following:

```bash
$ mkdir -p ~/open-source
$ cd ~/open-source
$ git clone https://github.com/synesissoftware/CLASP
$ cd ~/open-source/CLASP
$ ./prepare_cmake.sh -m
$ sudo cmake --install ./_build --config Release
```


#### Diagnosticism

**Diagnosticism** provides simple diagnostics utilities for C (and C++), including version-string helpers used by scratch and CLI programs.

**Diagnosticism** is obtained from **https://github.com/synesissoftware/Diagnosticism**, and it provides the means to install via **CMake**, as in the following:

```bash
$ mkdir -p ~/open-source
$ cd ~/open-source
$ git clone https://github.com/synesissoftware/Diagnosticism
$ cd ~/open-source/Diagnosticism
$ ./prepare_cmake.sh -m
$ sudo cmake --install ./_build --config Release
```


#### Pantheios - optional (locate-only for now)

**Pantheios** is a diagnostic logging API library. **libver** can locate it today but does not link it yet.

**Pantheios** is obtained from **https://github.com/synesissoftware/Pantheios**, and it provides the means to install via **CMake**, as in the following:

```bash
$ mkdir -p ~/open-source
$ cd ~/open-source
$ git clone https://github.com/synesissoftware/Pantheios
$ cd ~/open-source/Pantheios
$ ./prepare_cmake.sh -m
$ sudo cmake --install ./_build --config Release
```

Hint a local tree with `PANTHEIOS=…` or `CMAKE_PREFIX_PATH`. Require a successful locate with `-DLIBVER_WITH_PANTHEIOS=ON`.


#### STLSoft

The **STLSoft** libraries provide compiler/library discrimination and compatibility facilities, operating-system API façades, and extended components beyond the standard library.

**libver** requests **STLSoft** 1.11. Obtain it from **https://github.com/synesissoftware/STLSoft-1.11**, and install via **CMake**, as in the following:

```bash
$ mkdir -p ~/open-source
$ cd ~/open-source
$ git clone https://github.com/synesissoftware/STLSoft-1.11
$ cd ~/open-source/STLSoft-1.11
$ ./prepare_cmake.sh -m
$ sudo cmake --install ./_build --config Release
```

Alternatively, point CMake at an unpacked tree with the `STLSOFT` variable / environment variable (include path `${STLSOFT}/include`).


#### collect-c

**collect-c** provides C collection types used by the CLI / frontend stack.

**collect-c** is obtained from **https://github.com/synesissoftware/collect-c**, and it provides the means to install via **CMake**, as in the following:

```bash
$ mkdir -p ~/open-source
$ cd ~/open-source
$ git clone https://github.com/synesissoftware/collect-c
$ cd ~/open-source/collect-c
$ ./prepare_cmake.sh -m
$ sudo cmake --install ./_build --config Release
```

Hint a local build tree with `COLLECT_C=…` (for example `$HOME/dev/synesissoftware/forks/freelibs/collect/collect-c/_build`).


#### cstring

The **cstring** library provides resizeable C-style strings, and vectors of them.

**cstring** is obtained from **https://github.com/synesissoftware/cstring**, and it provides the means to install via **CMake**, as in the following:

```bash
$ mkdir -p ~/open-source
$ cd ~/open-source
$ git clone https://github.com/synesissoftware/cstring
$ cd ~/open-source/cstring
$ ./prepare_cmake.sh -m
$ sudo cmake --install ./_build --config Release
```


#### recls

**recls** (_recursive ls_) provides portable recursive filesystem search. It is a first-class **libver** dependency: used today by scratch **test.scratch.python2**, and expected again for core tree discovery.

**recls** is obtained from **https://github.com/synesissoftware/recls**, and it provides the means to install via **CMake**, as in the following:

```bash
$ mkdir -p ~/open-source
$ cd ~/open-source
$ git clone https://github.com/synesissoftware/recls
$ cd ~/open-source/recls
$ ./prepare_cmake.sh -m
$ sudo cmake --install ./_build --config Release
```

Hint a local build tree with `RECLS=…` or `CMAKE_PREFIX_PATH` (for example `$HOME/dev/synesissoftware/forks/freelibs/recls/recls/_build`).


#### sistools-common-c

**sistools-common-c** provides shared C helpers used by sistools-style CLI programs (and by **libver**'s CLI stubs).

**sistools-common-c** is obtained from **https://github.com/mwsis/sistools-common-c**, and it provides the means to install via **CMake**, as in the following:

```bash
$ mkdir -p ~/open-source
$ cd ~/open-source
$ git clone https://github.com/mwsis/sistools-common-c
$ cd ~/open-source/sistools-common-c
$ ./prepare_cmake.sh -m
$ sudo cmake --install ./_build --config Release
```

Hint a local build tree with `SISTOOLS_COMMON_C=…` (for example `$HOME/dev/sistools/sistools-common-c/_build`).


### Installation by other means

Dependencies in this subsection are not installed via **CMake**.


#### tomlc17

**tomlc17** is a TOML parser in C17. Upstream provides a Unix-oriented **Makefile** (no CMake package). **libver** discovers it with `find_path` / `find_library` (`tomlc17.h`, `libtomlc17`).

Obtain, test, build, and install as follows (operator layout under **`~/dev/3pty`**):

```bash
$ cd ~/dev/3pty
$ git clone -o cktan git@github.com:cktan/tomlc17.git ./cktan/tomlc17
$ cd cktan/tomlc17
$ unset DEBUG
$ make test
$ make
$ sudo make install prefix=/usr/local
```

Notes:

* **`unset DEBUG`** — when `DEBUG` is set in the environment, upstream's **Makefile** adds AddressSanitizer / UndefinedBehaviorSanitizer and lowers optimisation; leave it unset for a normal install;
* Install places headers (including **tomlc17.h**), **libtomlc17.a**, and **libtomlc17.pc** under the chosen `prefix` (here **`/usr/local`**);
* HTTPS clone alternative: `git clone https://github.com/cktan/tomlc17.git ./cktan/tomlc17`;
* Override discovery with `TOMLC17_INCLUDE_DIR` / `TOMLC17_LIBRARY` if needed;
* **Windows:** upstream CI and `make install` are Unix-oriented; there is no MSVC project. A portable story for Windows (e.g. building the amalgamated sources via this project's CMake) is still to be decided — see **TODO.md** / follow-up work.


## Required to build tests

Needed when `BUILD_TESTING` is ON (the default). Omit this group if you configure with testing disabled (`./prepare_cmake.sh -T` / `BUILD_TESTING=OFF`).

* [**xTests**](https://github.com/synesissoftware/xTests) — C/C++ unit-test harness;


### Installation by CMake


#### xTests

**xTests** is a simple, portable C/C++ testing library used by **test.unit.*** programs and **CTest**.

**xTests** is obtained from **https://github.com/synesissoftware/xTests**, and it provides the means to install via **CMake**, as in the following:

```bash
$ mkdir -p ~/open-source
$ cd ~/open-source
$ git clone https://github.com/synesissoftware/xTests
$ cd ~/open-source/xTests
$ ./prepare_cmake.sh -m
$ sudo cmake --install ./_build --config Release
```


### Installation by other means

There are currently no non-CMake dependencies required only for tests.


## Further help

If you cannot or will not use the install methods above and wish to use other means, we do not currently provide full instructions for that — because we cannot know what or how you wish to operate — but if you post a question (in **https://github.com/synesissoftware/libver/issues**) we will attempt to help you.


<!-- ########################### end of file ########################### -->
