# **libver** - FAQ <!-- omit in toc -->

The FAQ list is under (constant) development. If you post a question on the
Issues forum it will be used to create one.


## Table of Contents <!-- omit in toc -->

- [Q1: "How do I build libver?"](#q1-how-do-i-build-libver)
- [Q2: "What third-party dependencies do I need?"](#q2-what-third-party-dependencies-do-i-need)
- [Q3: "Do the CLIs discover a project's version yet?"](#q3-do-the-clis-discover-a-projects-version-yet)
- [Q4: "Where is the design documented?"](#q4-where-is-the-design-documented)


# FAQs: <!-- omit in toc -->


## Q1: "How do I build libver?"

See [INSTALL.md](./INSTALL.md) for the recommended **CMake** flow
(**prepare_cmake.sh**, then **build_cmake.sh**). Default options build the
static core, stub CLIs, unit smoke, and language scratches.


## Q2: "What third-party dependencies do I need?"

See [REQUISITES.md](./REQUISITES.md) for obtain / build / install steps.

Always: **Diagnosticism**, **recls**, **STLSoft**, **tomlc17**. Add the
**CLASP** / **collect-c** / **cstring** / **sistools-common-c** stack when
`BUILD_CLI` or `BUILD_FRONTENDS` is ON (the default). Add **xTests** when
`BUILD_TESTING` is ON (the default). **Pantheios** is locate-only today
and is not linked.


## Q3: "Do the CLIs discover a project's version yet?"

Phase 1 ships stub **libver** and **cargo-libver** binaries that honour
`--help` / `--version` only. Core `libver_find` does not walk a directory
yet. Language-specific extraction exists only in scratches under
**core/test/scratch/**. Phase 2 starts with fixtures and a Cargo backend;
see [docs/roadmap.md](./docs/roadmap.md).


## Q4: "Where is the design documented?"

See [docs/README.md](./docs/README.md) for architecture, C-API sketch,
detection precedence, and roadmap.


<!-- ########################### end of file ########################### -->
