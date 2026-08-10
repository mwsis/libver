# **libver** - TODO <!-- omit in toc -->


## Table of Contents <!-- omit in toc -->

- [Functional improvements](#functional-improvements)
- [Nonfunctional improvements](#nonfunctional-improvements)
- [Packaging improvements](#packaging-improvements)
- [Operator / rulesbase](#operator--rulesbase)


## Functional improvements

* [x] ~~~Unit smoke for stub `libver_find` (**test.unit.libver.find** / **CTest**)~~~ - ✅;
* [ ] Core C-API (`libver_find` / result model);
* [ ] Cargo backend (**Cargo.toml**);
* [ ] Zig backend (**build.zig.zon**);
* [ ] Shared **libver** CLI;
* [ ] **cargo-libver** frontend;
* [ ] Python / Ruby / C++ backends;


## Nonfunctional improvements

T.B.C.


## Packaging improvements

* [x] ~~~Boilerplate: **docs/** architecture set; Synesis markdown + CMake helper scripts from **cstring** exemplar~~~ - ✅;
* [x] ~~~Minimal buildable **CMake** tree (static **libver** stub)~~~ - ✅;
* [x] ~~~**run_all_unit_tests.sh** / **.cmd** and **run_all_scratch_tests.sh** (from **cstring**)~~~ - ✅;
* [ ] **run_all_examples.sh** / **generate_doxygen.sh** when examples/Doxygen exist;
* [ ] GitHub Actions (**ci.yml** + **ci-cell.yml**);
* [ ] INSTALL / FAQ filled beyond stubs;


## Operator / rulesbase

* [ ] Manually propagate local C/C++ block-header casing rule (**.cursor/rules/local-c-cpp-block-headers.mdc**) into the upstream freelibs/SISTrS rulesbase (prefer shared **c-cpp-standards.mdc**), then drop or thin the local elaboration after sync;
* [ ] Manually propagate local C/C++ type-bound declarator rule for `*` / `&` / `&&` (**.cursor/rules/local-c-cpp-local-declarators.mdc**) into the upstream freelibs/SISTrS rulesbase (prefer shared **c-cpp-standards.mdc**), then drop or thin the local elaboration after sync;
* [ ] Manually propagate local west-`const` pointer rule (**.cursor/rules/local-c-cpp-west-const.mdc**) into the upstream freelibs/SISTrS rulesbase (prefer shared **c-cpp-standards.mdc**), then drop or thin the local elaboration after sync;


<!-- ########################### end of file ########################### -->

