# **libver** Changes <!-- omit in toc -->


## 0.0.1 - 10th August 2026

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


## 0.0.0 - 10th August 2026

* Bootstrap repository layout (**core/**, **cli/**, **frontends/**, **docs/**, **scripts/**);
* Recorded architecture, C-API sketch, detection precedence, and roadmap under **docs/**;
* Brought Synesis C project boilerplate from the **cstring** exemplar (CMake helper scripts, markdown set, **.gitattributes**, editor settings);
* Cursor rules: local **upstream-source-of-truth** policy plus sync script for shared freelibs/SISTrS rules;


<!-- ########################### end of file ########################### -->
