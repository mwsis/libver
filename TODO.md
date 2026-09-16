# **libver** - TODO <!-- omit in toc -->


## Table of Contents <!-- omit in toc -->

- [Functional improvements](#functional-improvements)
- [Nonfunctional improvements](#nonfunctional-improvements)
- [Packaging improvements](#packaging-improvements)
- [Operator / rulesbase](#operator--rulesbase)


## Functional improvements

* [x] ~~~Unit smoke for stub `libver_find` (**test.unit.libver.find** / **CTest**)~~~ - ✅;
* [x] ~~~Scratch **test.scratch.python2**: **recls** recurse **setup.py** / **__init__.py**; `version=` / `__version__`; agree or drift~~~ - ✅;
* [x] ~~~Scratch **test.scratch.python3**: parse **pyproject.toml** `[project].version` via **tomlc17**~~~ - ✅;
* [x] ~~~Scratch **test.scratch.rust**: parse **Cargo.toml** `[package].version` via **tomlc17**~~~ - ✅;
* [x] ~~~Scratch **test.scratch.zig**: parse **build.zig.zon** `.version` (ZON line extract)~~~ - ✅;
* [x] ~~~Robust **build.zig.zon** `.version` extraction: small lexical scanner for the top-level field, comments, multiline whitespace, string decoding, and SemVer validation; a full ZON parser is unnecessary~~~ - ✅;
* [ ] Scratch **test.scratch.zip**;
* [ ] Core C-API warnings / conflict records (result model Phase 4);
* [x] ~~~Cargo backend (**Cargo.toml**)~~~ - ✅;
* [x] ~~~Zig backend (**build.zig.zon**)~~~ - ✅;
* [x] ~~~CLI stubs **libver** / **cargo-libver** (**CLASP** + **sistools-common-c**)~~~ - ✅;
* [ ] Shared **libver** CLI (real discovery behaviour);
* [ ] **cargo-libver** frontend (real discovery behaviour);
* [ ] Wire **Pantheios** logging; adopt **libCLImate** later;
* [ ] Python / Ruby / C++ backends;
* [ ] Consider embedding **TCC** (as a dynamic library) for C version checking — necessity not yet determined;


## Nonfunctional improvements

* [ ] Articulate and apply the **“always be abstracting”** principle (details T.B.C.);


## Packaging improvements

* [x] ~~~Boilerplate: **docs/** architecture set; Synesis markdown + CMake helper scripts from **cstring** exemplar~~~ - ✅;
* [x] ~~~Minimal buildable **CMake** tree (static **libver** stub)~~~ - ✅;
* [x] ~~~**run_all_unit_tests.sh** / **.cmd** and **run_all_scratch_tests.sh** (from **cstring**)~~~ - ✅;
* [ ] **run_all_examples.sh** / **generate_doxygen.sh** when examples/Doxygen exist;
* [ ] GitHub Actions (**ci.yml** + **ci-cell.yml**);
* [x] ~~~**REQUISITES.md** (deps obtain / build / install, including **tomlc17**)~~~ - ✅;
* [x] ~~~**INSTALL.md** / **FAQ.md** filled beyond stubs (link **REQUISITES.md**; stub CLI status)~~~ - ✅;


## Operator / rulesbase

* [ ] Manually propagate local C/C++ block-header casing rule (**.cursor/rules/local-c-cpp-block-headers.mdc**) into the upstream freelibs/SISTrS rulesbase (prefer shared **c-cpp-standards.mdc**), then drop or thin the local elaboration after sync;
* [ ] Manually propagate local C/C++ type-bound declarator rule for `*` / `&` / `&&` (**.cursor/rules/local-c-cpp-local-declarators.mdc**) into the upstream freelibs/SISTrS rulesbase (prefer shared **c-cpp-standards.mdc**), then drop or thin the local elaboration after sync;
* [ ] Manually propagate local west-`const` pointer rule (**.cursor/rules/local-c-cpp-west-const.mdc**) into the upstream freelibs/SISTrS rulesbase (prefer shared **c-cpp-standards.mdc**), then drop or thin the local elaboration after sync;
* [ ] Manually propagate local unit-test function naming rule (**.cursor/rules/local-c-cpp-test-function-names.mdc**: `TEST_` prefix; SHOUTING parts except construct names / language identifiers) into the upstream freelibs/SISTrS rulesbase (prefer shared **test-standards.mdc**), then drop or thin the local elaboration after sync;
* [ ] Capture **“always be abstracting”** as a local (then upstream) rule once the principle is explained;


<!-- ########################### end of file ########################### -->

