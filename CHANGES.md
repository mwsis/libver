# **libver** Changes <!-- omit in toc -->


## 0.0.1 - 10th August 2026

* Added unit smoke **test.unit.libver.find** (stub `libver_find` / init / null-result) registered with **CTest**;
* When **BUILD_TESTING** is ON, require **STLSoft** and **xTests**;
* **Diagnosticism** is required always (CLI + scratch), not only for tests;
* Added Synesis runners **run_all_unit_tests.sh** / **.cmd** and **run_all_scratch_tests.sh** (from **cstring**);
* CLI stubs **cli/libver** and **frontends/cargo-libver** via shared **cli/common/entry.c** (**CLASP**, **sistools-common-c**, **cstring**, **collect-c**);
* Pantheios: optional CMake locate (`LIBVER_WITH_PANTHEIOS`); not linked yet; **libCLImate** deferred;


## 0.0.0 - 10th August 2026

* Bootstrap repository layout (**core/**, **cli/**, **frontends/**, **docs/**, **scripts/**);
* Recorded architecture, C-API sketch, detection precedence, and roadmap under **docs/**;
* Brought Synesis C project boilerplate from the **cstring** exemplar (CMake helper scripts, markdown set, **.gitattributes**, editor settings);
* Cursor rules: local **upstream-source-of-truth** policy plus sync script for shared freelibs/SISTrS rules;


<!-- ########################### end of file ########################### -->
