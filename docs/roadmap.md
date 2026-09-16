# **libver** roadmap <!-- omit in toc -->

Status: planning checklist derived from the initial design conversations.


## Table of Contents <!-- omit in toc -->

- [Phase 0 — Lock names and contracts](#phase-0--lock-names-and-contracts)
- [Phase 1 — Scaffold (CMake only)](#phase-1--scaffold-cmake-only)
- [Phase 2 — First backends](#phase-2--first-backends)
- [Phase 3 — Public CLI surfaces](#phase-3--public-cli-surfaces)
- [Phase 4 — Harden the result model](#phase-4--harden-the-result-model)
- [Phase 5 — Python](#phase-5--python)
- [Phase 6 — Ruby](#phase-6--ruby)
- [Phase 7 — C/C++](#phase-7--cc)
- [Phase 8 — Packaging and bindings](#phase-8--packaging-and-bindings)


## Phase 0 — Lock names and contracts

* [x] Suite name **libver**;
* [x] Record architecture, C-API sketch, detection precedence, and this roadmap under **docs/**;
* [x] Finalise status codes and warning/conflict representation in [c-api.md](./c-api.md);


## Phase 1 — Scaffold (CMake only)

* [x] Root + nested **CMakeLists.txt** with options `BUILD_CLI`, `BUILD_FRONTENDS`, `BUILD_TESTING`;
* [x] Buildable static **libver** with stub `libver_find`;
* [x] **1.1** Wire unit/component test targets (smoke first) — **test.unit.libver.find** via **CTest**;
* [x] **1.2** Wire **cli/libver** and **frontends/cargo-libver** stubs (**CLASP** + **sistools-common-c**; **cstring** / **collect-c** linked; **Diagnosticism** always; **Pantheios** locate-only; **libCLImate** later);

Language-extraction scratches (**test.scratch.python2**, **python3**, **rust**, **versions**, **zig**) and **REQUISITES.md** landed as scaffold prep. They are not Phase 2 backends.


## Phase 2 — First backends

* [x] Fixtures: minimal Cargo project; minimal Zig zon project;
* [x] Cargo.toml version extraction;
* [x] `build.zig.zon` version extraction;
* [x] Detection + precedence tests (including no-match);


## Phase 3 — Public CLI surfaces

* [x] **cli/libver** — print language, version, source;
* [x] **frontends/cargo-libver** — Cargo-subcommand-friendly name/help (Cargo.toml only);
* [x] Manual smoke: `libver` and `cargo libver` with **cargo-libver** on `PATH`;


## Phase 4 — Harden the result model

* [x] Warnings/conflicts in the C-API;
* [x] Document CLI exit codes and optional machine-readable output (`--json`);


## Phase 5 — Python

* [x] Python scheme (`LIBVER_SCHEME_PYTHON`): **pyproject.toml** `[project].version`, then legacy **setup.py** / **`__init__.py`**;
* [x] Intra-ecosystem `inconsistent-sources` when Python version strings disagree;
* [x] Detection + precedence tests (Python-only, legacy agree/drift, Cargo-over-Python, nested `__init__.py` out of scope);


## Phase 6 — Ruby

* [ ] Ruby heuristics;


## Phase 7 — C/C++

* [ ] C/C++ heuristics;
* [ ] Consider embedding **TCC** (as a dynamic library) for C version checking — necessity not yet determined;


## Phase 8 — Packaging and bindings

* [ ] Rust crate wrapping core (crates.io / richer Cargo UX) when needed;
* [ ] Go module and/or .NET tool as thin installers/FFI;
* [ ] Root Cargo workspace only after a second Rust crate exists;


<!-- ########################### end of file ########################### -->
