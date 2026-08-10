# **libver** architecture <!-- omit in toc -->

Status: agreed for v0 planning (subject to refinement as implementation proceeds).


## Table of Contents <!-- omit in toc -->

- [Goal](#goal)
- [Product shape](#product-shape)
- [Core principles](#core-principles)
- [Repository layout](#repository-layout)
- [Build system](#build-system)
- [Naming](#naming)
- [Language frontends vs bindings](#language-frontends-vs-bindings)


## Goal

Given a directory (or the current working directory), discover the **definitive project version** for a recognised software ecosystem, and report the language/ecosystem, version, and winning source path (plus warnings/conflicts when needed).


## Product shape

* **Core library (`libver`)** — C library with a stable C-API; owns all detection and parsing;
* **Shared CLI (`libver`)** — thin C binary for language-neutral / CI use;
* **Ecosystem frontends** — thin entrypoints such as **cargo-libver** (`cargo libver`), later **go-libver**, **dotnet-libver**, etc.;
* **Bindings** (later) — in-process FFI wrappers for embedders; not a second implementation of discovery;


## Core principles

* Core owns detection; frontends do **not** reimplement parsers;
* The C-API is the stable contract;
* Frontends stay thin (argv, packaging, ecosystem UX);
* Prefer real mini-project fixtures over mocks;
* Detection precedence must be explicit and documented (see [detection-precedence.md](./detection-precedence.md));
* Early packaging: build core as a **static** library and link from CLIs/frontends;


## Repository layout

```text
libver/
├── core/                 # C library: include/libver/, src/, backends/, tests/
├── cli/libver/           # optional thin `libver` binary
├── frontends/
│   ├── cargo-libver/     # Cargo subcommand binary + standalone
│   ├── go-libver/        # later
│   └── dotnet-libver/    # later
├── bindings/             # later: rust / go / csharp / python FFI
├── cmake/                # shared CMake helpers
├── docs/                 # architecture, API, precedence, roadmap
└── scripts/              # project maintenance (e.g. Cursor rules sync)
```


## Build system

* **CMake** is the monorepo build spine (root + nested `CMakeLists.txt`);
* No top-level **Cargo.toml** / **go.mod** until a real package for that ecosystem exists;
* Language package manifests live **next to** the thing they publish;
* A Cargo workspace is added only after a second Rust crate appears;


## Naming

| Thing | Name |
| --- | --- |
| Suite / repository | **libver** |
| C library / header | **libver** / **libver.h** |
| Shared CLI | **libver** |
| Cargo frontend | **cargo-libver** (`cargo libver`) |


## Language frontends vs bindings

* Discovery CLIs (**libver**, **cargo-libver**, …) may be implemented in **C** for v0;
* Host-language wrappers exist for **packaging and UX** (`cargo install`, `dotnet tool`, …), not for reimplementing parsers;
* **Bindings** are for in-process callers;


<!-- ########################### end of file ########################### -->
