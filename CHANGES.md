# **libver** Changes <!-- omit in toc -->


## 0.0.2 - 16th September 2026

* `libver_find` probes **Cargo.toml** then **build.zig.zon** in the given directory (not recursive);
* Cargo backend reads `[package].version` via **tomlc17**;
* Zig backend reads top-level `.version` from **build.zig.zon** with a small ZON lexical scan (not TOML);
* Scheme result now has `patch` (was stub `path`) plus `source` path; pragmatic `LIBVER_RC_*` status codes;
* Fixtures under **core/test/fixtures/**; component tests **test.component.libver.find** (Cargo-only, Zig-only, Cargo-over-Zig, no-match, missing dir);
* Unit smoke **test.unit.libver.find** no longer expects a stub empty find;
* CLI **CLASP** table uses **specifications** (not obsolete **aliases**);
* **libver** CLI calls `libver_find` (all schemes; omitted directory is the process cwd);
* **cargo-libver** shares that entry but probes **Cargo.toml** only (skips Cargo's injected `libver` token);
* CLI prints `scheme` / `version` / `source`; process status follows `LIBVER_RC_*` (usage errors are 1);


## 0.0.1 - 16th September 2026

* Added unit smoke **test.unit.libver.find** (stub `libver_find` / init / null-result) registered with **CTest**;
* Scratch version printer is **test.scratch.versions** (`core/test/scratch/versions/main.c`), not a `libver`-named binary;
* When **BUILD_TESTING** is ON, require **STLSoft** and **xTests**;
* **Diagnosticism** is required always (CLI + scratch), not only for tests;
* **recls** is a first-class dependency (always; scratch **test.scratch.python2**, expected again in core);
* Added Synesis runners **run_all_unit_tests.sh** / **.cmd** and **run_all_scratch_tests.sh** (from **cstring**);
* CLI stubs **cli/libver** and **frontends/cargo-libver** via shared **cli/common/entry.c** (**CLASP**, **sistools-common-c**, **cstring**, **collect-c**);
* Pantheios: optional CMake locate (`LIBVER_WITH_PANTHEIOS`); not linked yet; **libCLImate** deferred;
* Scratch **test.scratch.python2**: recurse with **recls** for **setup.py** / **__init__.py**, extract `version=` / `__version__`, report agreement or drift;
* Scratch **test.scratch.python3**: read **pyproject.toml** `[project].version` with installed **tomlc17**;
* Scratch **test.scratch.rust**: read **Cargo.toml** `[package].version` with installed **tomlc17**;
* Scratch **test.scratch.zig**: read **build.zig.zon** `.version` (minimal ZON line extract; not TOML);
* Added **REQUISITES.md** (obtain / build / install for all third-party deps, including **tomlc17**);
* Closed out Phase 1 scaffold documentation (**TODO.md**, **INSTALL.md**, **FAQ.md**, **docs/roadmap.md**, **docs/architecture.md**);
* Optional **Pantheios** locate now exports `Pantheios_FOUND` from **cmake/LibverFindPackage.cmake** so `LIBVER_HAS_PANTHEIOS` is accurate (still not linked);


## 0.0.0 - 10th August 2026

* Bootstrap repository layout (**core/**, **cli/**, **frontends/**, **docs/**, **scripts/**);
* Recorded architecture, C-API sketch, detection precedence, and roadmap under **docs/**;
* Brought Synesis C project boilerplate from the **cstring** exemplar (CMake helper scripts, markdown set, **.gitattributes**, editor settings);
* Cursor rules: local **upstream-source-of-truth** policy plus sync script for shared freelibs/SISTrS rules;


<!-- ########################### end of file ########################### -->
