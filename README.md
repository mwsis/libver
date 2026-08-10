# **libver** <!-- omit in toc -->

Discover the definitive version of a software project from its directory.


[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](./LICENSE)
[![Last Commit](https://img.shields.io/github/last-commit/synesissoftware/libver)](https://github.com/synesissoftware/libver/commits)


## Table of Contents <!-- omit in toc -->

- [Introduction](#introduction)
- [Documentation](#documentation)
- [Building](#building)
  - [Prepare then build](#prepare-then-build)
  - [Prepare and build](#prepare-and-build)
- [Project Information](#project-information)


## Introduction

**libver** is a tool-suite with:

* a **C** core library and C-API (bindable from other languages);
* a shared **libver** CLI for language-neutral / CI use;
* thin ecosystem frontends (starting with **cargo-libver** for `cargo libver`);

The core detects the project type and extracts the definitive version using
language-specific rules (see [docs/detection-precedence.md](./docs/detection-precedence.md)).


## Documentation

* Design notes: [docs/README.md](./docs/README.md)
* Installation: [INSTALL.md](./INSTALL.md)
* FAQ: [FAQ.md](./FAQ.md)
* Changes: [CHANGES.md](./CHANGES.md)


## Building

### Prepare then build

```bash
$ ./prepare_cmake.sh
. . .

$ ./build_cmake.sh
. . .
```

### Prepare and build

```bash
$ ./prepare_cmake.sh -m
```

See [INSTALL.md](./INSTALL.md) for options.


## Project Information

* License: BSD 3-Clause — see [LICENSE](./LICENSE)
* Authors: [AUTHORS.md](./AUTHORS.md)
* News: [NEWS.md](./NEWS.md)
* TODO: [TODO.md](./TODO.md)
* Known issues: [KNOWN_ISSUES.md](./KNOWN_ISSUES.md)


<!-- ########################### end of file ########################### -->
