# **libver** detection precedence <!-- omit in toc -->

Status: agreed direction for v0; exact order may be tightened when fixtures land.


## Table of Contents <!-- omit in toc -->

- [v0 language set](#v0-language-set)
- [Version sources](#version-sources)
- [Proposed detection order](#proposed-detection-order)
- [Conflict policy](#conflict-policy)


## v0 language set

Implement first (clear single-file sources):

* **Cargo / Rust** — `Cargo.toml`
* **Zig** — `build.zig.zon`

Next backends:

* **Python 3** — `pyproject.toml`
* **Python 2** — `__init__.py` and/or `setup.py` (report inconsistency)
* **Ruby** — heuristics
* **C / C++** — heuristics and/or experimental preprocessor-only probes


## Version sources

| Ecosystem | Version source |
| --- | --- |
| Cargo / Rust | **Cargo.toml** |
| Zig | **build.zig.zon** |
| Python 3 | **pyproject.toml** |
| Python 2 | **`__init__.py`** and/or **setup.py**; report if inconsistent |
| Ruby | Heuristics |
| C / C++ | Established patterns and/or experimental preprocessor-only probes |


## Proposed detection order

When a tree could match multiple ecosystems, use an explicit ordered probe list. Working proposal for v0:

1. Cargo (`Cargo.toml`)
2. Zig (`build.zig.zon`)
3. Python (`pyproject.toml`, then legacy Python markers)
4. Ruby (heuristics)
5. C / C++ (heuristics)

Document any change to this order in this file and in **CHANGES.md**.


## Conflict policy

* Return **one winner** (language + version + source) according to precedence;
* Surface **warnings** when other ecosystems or inconsistent sources are also present;
* Do not silently pick an arbitrary winner when markers conflict within the same ecosystem;


<!-- ########################### end of file ########################### -->
