# **libver** detection precedence <!-- omit in toc -->

Status: v0 order in force (Cargo, then Zig, then Python). Ruby and C/C++
remain listed for planning only. Warning kinds are in [c-api.md](./c-api.md).


## Table of Contents <!-- omit in toc -->

- [v0 language set](#v0-language-set)
- [Version sources](#version-sources)
- [Detection scope](#detection-scope)
- [Proposed detection order](#proposed-detection-order)
- [Conflict policy](#conflict-policy)


## v0 language set

Implement first (clear single-file sources):

* **Cargo / Rust** — `Cargo.toml`
* **Zig** — `build.zig.zon`
* **Python** — `pyproject.toml`, then legacy **setup.py** / **`__init__.py`**

Next backends:

* **Ruby** — heuristics — Phase 6
* **C / C++** — heuristics and/or experimental preprocessor-only probes — Phase 7


## Version sources

| Ecosystem | Version source |
| --- | --- |
| Cargo / Rust | **Cargo.toml** |
| Zig | **build.zig.zon** |
| Python | **pyproject.toml** `[project].version`; else **setup.py** `version=` and/or **`__init__.py`** `__version__` / `version=` literals |
| Ruby | Heuristics |
| C / C++ | Established patterns and/or experimental preprocessor-only probes |


## Detection scope

`libver_find` probes **files in the given directory only** (not a recursive
tree walk). Cargo and Zig markers are `dir/Cargo.toml` and
`dir/build.zig.zon`.

Python v0 is the same directory plus **one-level package dirs**:

* Preferred: `dir/pyproject.toml` with a usable `[project].version`;
* Else legacy, in this winner order:
  1. `dir/setup.py`
  2. `dir/__init__.py`
  3. `dir/<name>/__init__.py` for each immediate subdirectory `<name>`,
     lexicographic by `<name>`;
* `dir/src/pkg/__init__.py` (two or more levels) is **out of scope**;
  recursive search may be added later;
* Poetry `tool.poetry.version`, **setup.cfg**, Hatch, and dynamic
  versioning are **out of scope**;


## Proposed detection order

When a tree could match multiple ecosystems, use an explicit ordered probe list. Working proposal for v0:

1. Cargo (`Cargo.toml`)
2. Zig (`build.zig.zon`)
3. Python (`pyproject.toml`, then legacy Python markers) — Phase 5
4. Ruby (heuristics) — Phase 6
5. C / C++ (heuristics) — Phase 7

Document any change to this order in this file and in **CHANGES.md**.


## Conflict policy

* Return **one winner** (language + version + source) according to precedence;
* Surface **warnings** when other **selected** ecosystems are also present
  (`LIBVER_WARNING_OTHER_ECOSYSTEM`); the winner is unchanged;
* Unselected schemes are ignored (a Cargo-only probe does not warn about
  Zig or Python);
* Intra-ecosystem inconsistency (`LIBVER_WARNING_INCONSISTENT_SOURCES`) is
  emitted for Python when collected version strings disagree; Cargo and Zig
  each have a single marker, so they do not emit that kind;
* Python still picks a **deterministic** winner: usable `pyproject.toml`
  `[project].version` first, else the first legacy source in the order
  above. Disagreeing sources are warnings, not a second scheme;
* A present `pyproject.toml` that fails to parse does **not** fall through
  to legacy. A present `[project].version` that is not usable SemVer does
  **not** fall through. A valid TOML file with no `[project].version`
  (string) does fall through to legacy;
* Parse failure of a **present winning** Cargo or Zig marker does **not**
  fall through;


<!-- ########################### end of file ########################### -->
